#include "RenderWindow.h"

#if VULKAN

#include "VulkanRenderWindow.h"

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
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.format == VK_FORMAT_B8G8R8_UNORM &&  
            availableFormat.format == VK_FORMAT_B8G8R8_UNORM && 
            availableFormat.format == VK_FORMAT_R8G8B8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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

void check_vk_result(VkResult err)
{
    if (err == 0) return;
    printf("VkResult %d\n", err);
    if (err < 0)
        abort();
}

const VulkanContext* GLFWVulkanRenderWindow::SetupVulkanContext(const char** extensions, uint32_t extensions_count)
{
    VulkanContext* vulkanContext = new VulkanContext();

    VkResult err;

    // Create Vulkan Instance
    {
        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.enabledExtensionCount = extensions_count;
        create_info.ppEnabledExtensionNames = extensions;

        // Create Vulkan Instance without any debug feature
        err = vkCreateInstance(&create_info, vulkanContext->m_Allocator, &vulkanContext->m_Instance);
        check_vk_result(err);
    }

    // Select GPU
    {
        uint32_t gpu_count;
        err = vkEnumeratePhysicalDevices(vulkanContext->m_Instance, &gpu_count, NULL);
        check_vk_result(err);
        BLA_ASSERT(gpu_count > 0);

        VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
        err = vkEnumeratePhysicalDevices(vulkanContext->m_Instance, &gpu_count, gpus);
        check_vk_result(err);

        // If a number >1 of GPUs got reported, you should find the best fit GPU for your purpose
        // e.g. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU if available, or with the greatest memory available, etc.
        // for sake of simplicity we'll just take the first one, assuming it has a graphics queue family.
        vulkanContext->m_PhysicalDevice = gpus[0];
        free(gpus);
    }

    // Select graphics queue family
    {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext->m_PhysicalDevice, &count, NULL);
        VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
        vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext->m_PhysicalDevice, &count, queues);
        for (uint32_t i = 0; i < count; i++)
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                vulkanContext->m_QueueFamily = i;
                break;
            }
        free(queues);
        BLA_ASSERT(vulkanContext->m_QueueFamily != (uint32_t)-1);
    }

    // Create Logical Device (with 1 queue)
    {
        int device_extension_count = 1;
        const char* device_extensions[] = { "VK_KHR_swapchain" };
        const float queue_priority[] = { 1.0f };
        VkDeviceQueueCreateInfo queue_info[1] = {};
        queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[0].queueFamilyIndex = vulkanContext->m_QueueFamily;
        queue_info[0].queueCount = 1;
        queue_info[0].pQueuePriorities = queue_priority;
        VkDeviceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
        create_info.pQueueCreateInfos = queue_info;
        create_info.enabledExtensionCount = device_extension_count;
        create_info.ppEnabledExtensionNames = device_extensions;
        err = vkCreateDevice(vulkanContext->m_PhysicalDevice, &create_info, vulkanContext->m_Allocator, &vulkanContext->m_Device);
        check_vk_result(err);
        vkGetDeviceQueue(vulkanContext->m_Device, vulkanContext->m_QueueFamily, 0, &vulkanContext->m_Queue);
    }

    // Create Descriptor Pool
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * ArraySize(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)ArraySize(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        err = vkCreateDescriptorPool(vulkanContext->m_Device, &pool_info, vulkanContext->m_Allocator, &vulkanContext->m_DescriptorPool);
        check_vk_result(err);
    }

    return vulkanContext;
}

GLFWVulkanRenderWindow::~GLFWVulkanRenderWindow()
{
    vkDestroySurfaceKHR(m_vulkanContext->m_Instance, m_vulkanWindowInfo->Surface, nullptr);
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
    m_vulkanContext = SetupVulkanContext(extensions, extensions_count);

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(m_vulkanContext->m_Instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        //TODO: Remove runtime_error throwing
        throw std::runtime_error("failed to create window surface!");
    }

    m_vulkanWindowInfo = new VulkanWindowInfo(surface);
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
        m_vulkanWindowInfo->FrameIndex = 0;
    }

    VkSemaphore render_complete_semaphore = m_vulkanWindowInfo->FrameSemaphores[m_vulkanWindowInfo->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &m_vulkanWindowInfo->Swapchain;
    info.pImageIndices = &m_vulkanWindowInfo->FrameIndex;
    VkResult err = vkQueuePresentKHR(m_vulkanContext->m_Queue, &info);
    check_vk_result(err);
    m_vulkanWindowInfo->SemaphoreIndex = (m_vulkanWindowInfo->SemaphoreIndex + 1) % m_vulkanWindowInfo->ImageCount; // Now we can use the next set of semaphores
}

const VulkanContext* GLFWVulkanRenderWindow::GetVulkanContext() const
{
    return m_vulkanContext;
}

VulkanWindowInfo* GLFWVulkanRenderWindow::GetVulkanWindowInfo() const
{
    return m_vulkanWindowInfo;
}

void GLFWVulkanRenderWindow::CreateSwapChain()
{
    VkResult err;

    //////////////////////////////////////////////////////////////////////
    // Clean shit up
    VkSwapchainKHR oldSwapchain = m_vulkanWindowInfo->Swapchain;

    // this is a blocking wait to collect frames in flight ...
    err = vkDeviceWaitIdle(m_vulkanContext->m_Device);
    check_vk_result(err);

    DestroySwapChainAndCommandBuffers();

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_vulkanContext->m_PhysicalDevice, m_vulkanWindowInfo->Surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(m_height, m_width, swapChainSupport.capabilities);

    m_vulkanWindowInfo->ImageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && m_vulkanWindowInfo->ImageCount > swapChainSupport.capabilities.maxImageCount)
    {
        m_vulkanWindowInfo->ImageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = m_vulkanWindowInfo->Surface;
    swapChainCreateInfo.oldSwapchain = oldSwapchain;
    swapChainCreateInfo.minImageCount = m_vulkanWindowInfo->ImageCount;
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
    err = vkCreateSwapchainKHR(m_vulkanContext->m_Device, &swapChainCreateInfo, nullptr, &swapChain);
    check_vk_result(err);

    m_vulkanWindowInfo->Swapchain = swapChain;

    vkGetSwapchainImagesKHR(m_vulkanContext->m_Device, swapChain, &m_vulkanWindowInfo->ImageCount, nullptr);

    {
        blaVector<VkImage> swapChainImages;
        swapChainImages.resize(m_vulkanWindowInfo->ImageCount);
        vkGetSwapchainImagesKHR(m_vulkanContext->m_Device, swapChain, &m_vulkanWindowInfo->ImageCount, swapChainImages.data());

        m_vulkanWindowInfo->SurfaceFormat = surfaceFormat;
        m_vulkanWindowInfo->Extent = extent;

        m_vulkanWindowInfo->Frames.resize(m_vulkanWindowInfo->ImageCount);
        for (blaU32 i = 0; i < m_vulkanWindowInfo->ImageCount; i++)
        {
            m_vulkanWindowInfo->Frames[i].Backbuffer = swapChainImages[i];
        }
    }
    
    m_vulkanWindowInfo->FrameSemaphores.resize(m_vulkanWindowInfo->ImageCount);

    if (oldSwapchain)
        vkDestroySwapchainKHR(m_vulkanContext->m_Device, oldSwapchain, nullptr);

    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_vulkanWindowInfo->SurfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        for (size_t i = 0; i < m_vulkanWindowInfo->ImageCount; i++)
        {
            createInfo.image = m_vulkanWindowInfo->Frames[i].Backbuffer;
            if (vkCreateImageView(m_vulkanContext->m_Device, &createInfo, nullptr, &m_vulkanWindowInfo->Frames[i].BackbufferView) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    {
        VkImageView attachment[1];
        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = m_vulkanWindowInfo->ImGuiRenderPass;
        info.attachmentCount = 1;
        info.pAttachments = attachment;
        info.width = m_vulkanWindowInfo->Extent.width;
        info.height = m_vulkanWindowInfo->Extent.height;
        info.layers = 1;
        for (uint32_t i = 0; i < m_vulkanWindowInfo->ImageCount; i++)
        {
            attachment[0] = m_vulkanWindowInfo->Frames[i].BackbufferView;
            VkResult err = vkCreateFramebuffer(m_vulkanContext->m_Device, &info, nullptr, &m_vulkanWindowInfo->Frames[i].Framebuffer);
            check_vk_result(err);
        }
    }
}

void GLFWVulkanRenderWindow::CreateSwapChainCommandBuffers()
{
    // Create Command Buffers
    VkResult err;
    for (uint32_t i = 0; i < m_vulkanWindowInfo->ImageCount; i++)
    {
        {
            VkCommandPoolCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            info.queueFamilyIndex = m_vulkanContext->m_QueueFamily;
            err = vkCreateCommandPool(m_vulkanContext->m_Device, &info, nullptr, &m_vulkanWindowInfo->Frames[i].CommandPool);
            check_vk_result(err);
        }
        {
            VkCommandBufferAllocateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.commandPool = m_vulkanWindowInfo->Frames[i].CommandPool;
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            info.commandBufferCount = 1;
            err = vkAllocateCommandBuffers(m_vulkanContext->m_Device, &info, &m_vulkanWindowInfo->Frames[i].CommandBuffer);
            check_vk_result(err);
        }
        {
            VkFenceCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            err = vkCreateFence(m_vulkanContext->m_Device, &info, nullptr, &m_vulkanWindowInfo->Frames[i].Fence);
            check_vk_result(err);
        }
        {
            VkSemaphoreCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            err = vkCreateSemaphore(m_vulkanContext->m_Device, &info, nullptr, &m_vulkanWindowInfo->FrameSemaphores[i].ImageAcquiredSemaphore);
            check_vk_result(err);
            err = vkCreateSemaphore(m_vulkanContext->m_Device, &info, nullptr, &m_vulkanWindowInfo->FrameSemaphores[i].RenderCompleteSemaphore);
            check_vk_result(err);
        }
    }
}

void GLFWVulkanRenderWindow::DestroySwapChainAndCommandBuffers()
{
    for (uint32_t i = 0; i < m_vulkanWindowInfo->ImageCount; i++)
    {
        // Destroy Frame
        {
            VulkanFrameContext* frame = &m_vulkanWindowInfo->Frames[i];
            vkDestroyFence(m_vulkanContext->m_Device, frame->Fence, nullptr);
            vkFreeCommandBuffers(m_vulkanContext->m_Device, frame->CommandPool, 1, &frame->CommandBuffer);
            vkDestroyCommandPool(m_vulkanContext->m_Device, frame->CommandPool, nullptr);
            frame->Fence = VK_NULL_HANDLE;
            frame->CommandBuffer = VK_NULL_HANDLE;
            frame->CommandPool = VK_NULL_HANDLE;

            vkDestroyImageView(m_vulkanContext->m_Device, frame->BackbufferView, nullptr);
            vkDestroyFramebuffer(m_vulkanContext->m_Device, frame->Framebuffer, nullptr);
        }

        // Destroy Frame Semaphore
        {
            VulkanFrameSemaphore* frame = &m_vulkanWindowInfo->FrameSemaphores[i];
            vkDestroySemaphore(m_vulkanContext->m_Device, frame->ImageAcquiredSemaphore, nullptr);
            vkDestroySemaphore(m_vulkanContext->m_Device, frame->RenderCompleteSemaphore, nullptr);
            frame->ImageAcquiredSemaphore = frame->RenderCompleteSemaphore = VK_NULL_HANDLE;
        }
    }
    m_vulkanWindowInfo->Frames.clear();
    m_vulkanWindowInfo->FrameSemaphores.clear();
    m_vulkanWindowInfo->ImageCount = 0;
}

#endif
