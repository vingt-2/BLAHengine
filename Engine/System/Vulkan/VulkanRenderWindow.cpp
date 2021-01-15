#include "Context.h"
#include "System/RenderWindow.h"
// TODO: Fix this spaghetto
#include "Rendering/Gpu/Interface.h"
#include "Rendering/Gpu/Vulkan.h"
#include <GLFW/glfw3.h>

using namespace BLA;
using namespace System;

bool g_enableValidationLayers = false;

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

GLFWVulkanRenderWindow::~GLFWVulkanRenderWindow()
{
    vkDestroySurfaceKHR(m_vulkanContext->m_instance, m_vulkanWindowInfo->m_surface, nullptr);
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) 
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkDebugUtilsMessengerEXT debugMessenger;

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void setupDebugMessenger(VkInstance instance)
{
    if (!g_enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) 
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0) 
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
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

    blaVector<const char*> allExtensions(extensions_count);
    memcpy_s(allExtensions.data(), sizeof(const char*) * extensions_count, extensions, extensions_count * sizeof(const char*));

    // allExtensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    
    if(g_enableValidationLayers)
    {
        allExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        allExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        m_vulkanContext = new Vulkan::Context(allExtensions, validationLayers);
        
        setupDebugMessenger(m_vulkanContext->m_instance);
    }
    else
    {
        m_vulkanContext = new Vulkan::Context(allExtensions, blaVector<const char*>());
    }
    
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(m_vulkanContext->m_instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        //TODO: Remove runtime_error throwing
        throw std::runtime_error("failed to create window surface!");
    }

    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_vulkanContext->m_physicalDevice, m_vulkanContext->m_queueFamily, surface, &res);
    if (res != VK_TRUE)
    {
        throw std::runtime_error("Surface unsupported !");
        return;
    }


    m_vulkanWindowInfo = new Vulkan::WindowInfo(surface);

    m_vulkanContext->m_window = m_vulkanWindowInfo;
    
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

    Gpu::Interface::AssignSingletonInstance(new Gpu::Vulkan(m_vulkanContext));
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
        m_vulkanWindowInfo->m_semaphoreIndex = 0;
        m_updated = true;
        return;
    }

    VkSemaphore render_complete_semaphore = m_vulkanWindowInfo->m_frameSemaphores[m_vulkanWindowInfo->m_semaphoreIndex].m_renderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &m_vulkanWindowInfo->m_swapchain;
    info.pImageIndices = &m_vulkanWindowInfo->m_frameIndex;
    VkResult err = vkQueuePresentKHR(m_vulkanContext->m_queue, &info);
    Vulkan::Context::HandleError(err);
    m_vulkanWindowInfo->m_semaphoreIndex = (m_vulkanWindowInfo->m_semaphoreIndex + 1) % m_vulkanWindowInfo->m_imageCount; // Now we can use the next set of semaphores

    m_vulkanWindowInfo->m_frameIndex = (m_vulkanWindowInfo->m_frameIndex + 1) % m_vulkanWindowInfo->m_imageCount;
}

Vulkan::Context* GLFWVulkanRenderWindow::GetVulkanInterface() const
{
    return m_vulkanContext;
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
    err = vkDeviceWaitIdle(m_vulkanContext->m_device);
    Vulkan::Context::HandleError(err);

    DestroySwapChainAndCommandBuffers();

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_vulkanContext->m_physicalDevice, m_vulkanWindowInfo->m_surface);

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
    err = vkCreateSwapchainKHR(m_vulkanContext->m_device, &swapChainCreateInfo, nullptr, &swapChain);
    Vulkan::Context::HandleError(err);

    m_vulkanWindowInfo->m_swapchain = swapChain;

    vkGetSwapchainImagesKHR(m_vulkanContext->m_device, swapChain, &m_vulkanWindowInfo->m_imageCount, nullptr);

    {
        blaVector<VkImage> swapChainImages;
        swapChainImages.resize(m_vulkanWindowInfo->m_imageCount);
        Vulkan::Context::HandleError(vkGetSwapchainImagesKHR(
            m_vulkanContext->m_device, 
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
        vkDestroySwapchainKHR(m_vulkanContext->m_device, oldSwapchain, nullptr);
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
            if (vkCreateImageView(m_vulkanContext->m_device, &createInfo, nullptr, &m_vulkanWindowInfo->m_frames[i].m_backBufferView) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
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
            info.queueFamilyIndex = m_vulkanContext->m_queueFamily;
            err = vkCreateCommandPool(m_vulkanContext->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frames[i].m_commandPool);
            Vulkan::Context::HandleError(err);
        }
        {
            VkCommandBufferAllocateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.commandPool = m_vulkanWindowInfo->m_frames[i].m_commandPool;
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            info.commandBufferCount = 1;
            err = vkAllocateCommandBuffers(m_vulkanContext->m_device, &info, &m_vulkanWindowInfo->m_frames[i].m_commandBuffer);
            Vulkan::Context::HandleError(err);
        }
        {
            VkFenceCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            err = vkCreateFence(m_vulkanContext->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frames[i].m_imageFence);
            Vulkan::Context::HandleError(err);
        }
        {
            VkSemaphoreCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            err = vkCreateSemaphore(m_vulkanContext->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frameSemaphores[i].m_imageAcquiredSemaphore);
            Vulkan::Context::HandleError(err);
            err = vkCreateSemaphore(m_vulkanContext->m_device, &info, nullptr, &m_vulkanWindowInfo->m_frameSemaphores[i].m_renderCompleteSemaphore);
            Vulkan::Context::HandleError(err);
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
            vkDestroyFence(m_vulkanContext->m_device, frame->m_imageFence, nullptr);
            vkFreeCommandBuffers(m_vulkanContext->m_device, frame->m_commandPool, 1, &frame->m_commandBuffer);
            vkDestroyCommandPool(m_vulkanContext->m_device, frame->m_commandPool, nullptr);
            frame->m_imageFence = VK_NULL_HANDLE;
            frame->m_commandBuffer = VK_NULL_HANDLE;
            frame->m_commandPool = VK_NULL_HANDLE;

            vkDestroyImageView(m_vulkanContext->m_device, frame->m_backBufferView, nullptr);
            vkDestroyFramebuffer(m_vulkanContext->m_device, frame->m_framebuffer, nullptr);
        }

        // Destroy Frame Semaphore
        {
            Vulkan::FrameSemaphore* frame = &m_vulkanWindowInfo->m_frameSemaphores[i];
            vkDestroySemaphore(m_vulkanContext->m_device, frame->m_imageAcquiredSemaphore, nullptr);
            vkDestroySemaphore(m_vulkanContext->m_device, frame->m_renderCompleteSemaphore, nullptr);
            frame->m_imageAcquiredSemaphore = frame->m_renderCompleteSemaphore = VK_NULL_HANDLE;
        }
    }
    m_vulkanWindowInfo->m_frames.clear();
    m_vulkanWindowInfo->m_frameSemaphores.clear();
    m_vulkanWindowInfo->m_imageCount = 0;
}