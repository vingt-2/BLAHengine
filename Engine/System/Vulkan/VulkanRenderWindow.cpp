#include "System/RenderWindow.h"
#include "VulkanInterface.h"

using namespace BLA;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    blaVector<VkSurfaceFormatKHR> formats;
    blaVector<VkPresentModeKHR> presentModes;
};

static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) 
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if ((availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) || 
            (availableFormat.format == VK_FORMAT_B8G8R8_UNORM) ||
            (availableFormat.format == VK_FORMAT_R8G8B8_UNORM) ||
            (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseSwapExtent(blaU32 desiredHeight, blaU32 desiredWidth, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = { desiredWidth, desiredHeight };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

Vulkan::Interface* GLFWVulkanRenderWindow::SetupVulkanInterface(const char** extensions, uint32_t extensions_count)
{
    Vulkan::Interface* vulkanInterface = new Vulkan::Interface(extensions, extensions_count);
    return vulkanInterface;
}

GLFWVulkanRenderWindow::~GLFWVulkanRenderWindow()
{
    vkDestroySurfaceKHR(m_vulkanInterface->m_instance, m_vulkanWindowInfo->m_surface, nullptr);
}

void GLFWVulkanRenderWindow::CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        return;
    }

    m_isFullscreen = isFullScreen;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Open a window and create its OpenGL context
    GLFWmonitor* monitor = nullptr;

    if (isFullScreen)
    {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

        // Set window size to screen proportions
        m_width = videoMode->width;
        m_height = videoMode->height;

        m_isFullscreen = true;
    }
    else
    {
        m_width = sizeX;
        m_height = sizeY;
    }

    GLFWwindow* window = glfwCreateWindow(m_width, m_height, windowTitle.c_str(), monitor, NULL);

    // Setup Vulkan
    if (!glfwVulkanSupported())
    {
        printf("GLFW: Vulkan Not Supported\n");
        return;
    }
    uint32_t extensions_count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    m_vulkanInterface = new Vulkan::Interface(extensions, extensions_count);

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(m_vulkanInterface->m_instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        //TODO: Remove runtime_error throwing
        throw std::runtime_error("failed to create window surface!");
    }

    m_vulkanWindowInfo = new Vulkan::WindowInfo(surface);
    CreateSwapChain();
    CreateSwapChainCommandBuffers();

    ms_glfwRenderWindowInstanced.push_back(this);

    glfwSetWindowTitle(window, windowTitle.c_str());

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(window, m_width / 2, m_height / 2);

    glfwSetMouseButtonCallback(window, GLFWMouseButtonCallBack);
    glfwSetCursorPosCallback(window, GLFWMouseCursorPosCallBack);
    glfwSetKeyCallback(window, GLFWKeyboardCallBack);
    glfwSetScrollCallback(window, (GLFWscrollfun)GLFWMouseWheelCallback);

    glfwSetDropCallback(window, (GLFWdropfun)GLFWDragAndDropCallBack);

    m_glfwWindow = window;
}

void GLFWVulkanRenderWindow::UpdateWindowAndBuffers()
{
    int w = m_width;
    int h = m_height;

    GLFWRenderWindow::UpdateWindowAndBuffers();

    if (w != m_width || h != m_height)
    {
        CreateSwapChain();
        CreateSwapChainCommandBuffers();
        m_vulkanWindowInfo->m_frameIndex = 0;
    }

    VkSemaphore render_complete_semaphore = m_vulkanWindowInfo->m_frameSemaphores[m_vulkanWindowInfo->m_semaphoreIndex].m_renderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &m_vulkanWindowInfo->m_swapchain;
    info.pImageIndices = &m_vulkanWindowInfo->m_frameIndex;
    VkResult err = vkQueuePresentKHR(m_vulkanInterface->m_queue, &info);
    Vulkan::Interface::HandleError(err);
    m_vulkanWindowInfo->m_semaphoreIndex = (m_vulkanWindowInfo->m_semaphoreIndex + 1) % m_vulkanWindowInfo->m_imageCount; // Now we can use the next set of semaphores
}

Vulkan::Interface* GLFWVulkanRenderWindow::GetVulkanInterface() const
{
    return m_vulkanInterface;
}

Vulkan::WindowInfo* GLFWVulkanRenderWindow::GetVulkanWindowInfo() const
{
    return m_vulkanWindowInfo;
}

void GLFWVulkanRenderWindow::CreateSwapChain()
{
    VkResult err;

    //////////////////////////////////////////////////////////////////////
    // Clean shit up
    VkSwapchainKHR oldSwapchain = m_vulkanWindowInfo->m_swapchain;

    // this is a blocking wait to collect frames in flight ...
    err = vkDeviceWaitIdle(m_vulkanInterface->m_device);
    Vulkan::Interface::HandleError(err);

    DestroySwapChainAndCommandBuffers();

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_vulkanInterface->m_physicalDevice, m_vulkanWindowInfo->m_surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(m_height, m_width, swapChainSupport.capabilities);

    m_vulkanWindowInfo->m_imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && m_vulkanWindowInfo->m_imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        m_vulkanWindowInfo->m_imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = m_vulkanWindowInfo->m_surface;
    swapChainCreateInfo.oldSwapchain = oldSwapchain;
    swapChainCreateInfo.minImageCount = m_vulkanWindowInfo->m_imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;


    //TODO: Interesting
    /*uint32_t queueFamilyIndices[] = { (uint32_t)queueFamilies.graphicsQueue, (uint32_t)queueFamilies.presentationQueue };

    if (queueFamilies.graphicsQueue != queueFamilies.presentationQueue)
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }*/

    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;

    VkSwapchainKHR swapChain;
    err = vkCreateSwapchainKHR(m_vulkanInterface->m_device, &swapChainCreateInfo, nullptr, &swapChain);
    Vulkan::Interface::HandleError(err);

    m_vulkanWindowInfo->m_swapchain = swapChain;

    vkGetSwapchainImagesKHR(m_vulkanInterface->m_device, swapChain, &m_vulkanWindowInfo->m_imageCount, nullptr);

    {
        blaVector<VkImage> swapChainImages;
        swapChainImages.resize(m_vulkanWindowInfo->m_imageCount);
        Vulkan::Interface::HandleError(vkGetSwapchainImagesKHR(
            m_vulkanInterface->m_device, 
            swapChain, 
            &m_vulkanWindowInfo->m_imageCount, 
            swapChainImages.data()));

        m_vulkanWindowInfo->m_surfaceFormat = surfaceFormat;
        m_vulkanWindowInfo->m_extent = extent;

        m_vulkanWindowInfo->m_frames.resize(m_vulkanWindowInfo->m_imageCount);
        for (blaU32 i = 0; i < m_vulkanWindowInfo->m_imageCount; i++)
        {
            m_vulkanWindowInfo->m_frames[i].m_backbuffer = swapChainImages[i];
        }
    }
    
    m_vulkanWindowInfo->m_frameSemaphores.resize(m_vulkanWindowInfo->m_imageCount);

    if (oldSwapchain)
    {
        vkDestroySwapchainKHR(m_vulkanInterface->m_device, oldSwapchain, nullptr);
    }
        
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_vulkanWindowInfo->m_surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        for (size_t i = 0; i < m_vulkanWindowInfo->m_imageCount; i++)
        {
            createInfo.image = m_vulkanWindowInfo->m_frames[i].m_backbuffer;
            if (vkCreateImageView(m_vulkanInterface->m_device, &createInfo, nullptr, &m_vulkanWindowInfo->m_frames[i].m_backBufferView) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    {
        VkImageView attachment[1];
        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = m_vulkanWindowInfo->m_renderWindowPresentationPass;
        info.attachmentCount = 1;
        info.pAttachments = attachment;
        info.width = m_vulkanWindowInfo->m_extent.width;
        info.height = m_vulkanWindowInfo->m_extent.height;
        info.layers = 1;
        for (uint32_t i = 0; i < m_vulkanWindowInfo->m_imageCount; i++)
        {
            attachment[0] = m_vulkanWindowInfo->m_frames[i].m_backBufferView;
            VkResult err = vkCreateFramebuffer(m_vulkanInterface->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frames[i].m_framebuffer);
            Vulkan::Interface::HandleError(err);
        }
    }
}

void GLFWVulkanRenderWindow::CreateSwapChainCommandBuffers()
{
    // Create Command Buffers
    VkResult err;
    for (uint32_t i = 0; i < m_vulkanWindowInfo->m_imageCount; i++)
    {
        {
            VkCommandPoolCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            info.queueFamilyIndex = m_vulkanInterface->m_queueFamily;
            err = vkCreateCommandPool(m_vulkanInterface->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frames[i].m_commandPool);
            Vulkan::Interface::HandleError(err);
        }
        {
            VkCommandBufferAllocateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.commandPool = m_vulkanWindowInfo->m_frames[i].m_commandPool;
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            info.commandBufferCount = 1;
            err = vkAllocateCommandBuffers(m_vulkanInterface->m_device, &info, &m_vulkanWindowInfo->m_frames[i].m_commandBuffer);
            Vulkan::Interface::HandleError(err);
        }
        {
            VkFenceCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            err = vkCreateFence(m_vulkanInterface->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frames[i].m_imageFence);
            Vulkan::Interface::HandleError(err);
        }
        {
            VkSemaphoreCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            err = vkCreateSemaphore(m_vulkanInterface->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frameSemaphores[i].m_imageAcquiredSemaphore);
            Vulkan::Interface::HandleError(err);
            err = vkCreateSemaphore(m_vulkanInterface->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frameSemaphores[i].m_renderCompleteSemaphore);
            Vulkan::Interface::HandleError(err);
        }
    }
}

void GLFWVulkanRenderWindow::DestroySwapChainAndCommandBuffers()
{
    for (uint32_t i = 0; i < m_vulkanWindowInfo->m_imageCount; i++)
    {
        // Destroy Frame
        {
            Vulkan::FrameContext* frame = &m_vulkanWindowInfo->m_frames[i];
            vkDestroyFence(m_vulkanInterface->m_device, frame->m_imageFence, nullptr);
            vkFreeCommandBuffers(m_vulkanInterface->m_device, frame->m_commandPool, 1, &frame->m_commandBuffer);
            vkDestroyCommandPool(m_vulkanInterface->m_device, frame->m_commandPool, nullptr);
            frame->m_imageFence = VK_NULL_HANDLE;
            frame->m_commandBuffer = VK_NULL_HANDLE;
            frame->m_commandPool = VK_NULL_HANDLE;

            vkDestroyImageView(m_vulkanInterface->m_device, frame->m_backBufferView, nullptr);
            vkDestroyFramebuffer(m_vulkanInterface->m_device, frame->m_framebuffer, nullptr);
        }

        // Destroy Frame Semaphore
        {
            Vulkan::FrameSemaphore* frame = &m_vulkanWindowInfo->m_frameSemaphores[i];
            vkDestroySemaphore(m_vulkanInterface->m_device, frame->m_imageAcquiredSemaphore, nullptr);
            vkDestroySemaphore(m_vulkanInterface->m_device, frame->m_renderCompleteSemaphore, nullptr);
            frame->m_imageAcquiredSemaphore = frame->m_renderCompleteSemaphore = VK_NULL_HANDLE;
        }
    }
    m_vulkanWindowInfo->m_frames.clear();
    m_vulkanWindowInfo->m_frameSemaphores.clear();
    m_vulkanWindowInfo->m_imageCount = 0;
}