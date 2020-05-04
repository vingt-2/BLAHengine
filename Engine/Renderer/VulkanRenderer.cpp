#define GLFW_INCLUDE_VULKAN

#include "RenderBackend.h"
#include "System/Console.h"
#include "Renderer/MeshRendererComponent.h"
#include "Renderer/PointLightComponent.h"
#include "Renderer/LightRender.h"

#include <optional>
#include "VulkanRenderer.h"
#include <set>

using namespace BLA;

class BLAVulkanHelpers 
{
public:

    struct VkQueueFamilyIndices
    {
        blaS32 graphicsQueue = -1;
        blaS32 presentationQueue = -1;

        bool AllQueuesFilled()
        {
            return graphicsQueue != -1 && presentationQueue != -1;
        }
    };

    static bool checkDeviceExtensionSupport(VkPhysicalDevice device, blaVector<const char*> deviceExtensions)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) 
        {
            requiredExtensions.erase(extension.extensionName);
        }
        return requiredExtensions.empty();
    }

    static VkQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        VkQueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.graphicsQueue = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) 
            {
                indices.presentationQueue = i;
            }

            if(indices.AllQueuesFilled()) 
            {
                break;
            }

            i++;
        }

        return indices;
    }

    static bool IsSuitableDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        /*VkQueueFamilyIndices q = FindQueueFamilies(device, surface);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;*/
    
        return true;
    }

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

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
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

    static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
    {
        if (capabilities.currentExtent.width != UINT32_MAX) 
        {
            return capabilities.currentExtent;
        }

        VkExtent2D actualExtent = { 1000, 700};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
};

void VulkanRenderer::InitializeRenderer(RenderWindow* renderWindow, RenderingManager* renderingManager, DebugRenderingManager* debugRenderManager)
{
    // Create VkInstance
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "BLAEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    VkInstance vulkanInstance;

    if(vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS)
    {
        Console::LogError("Failed to create a VkInstance object");
        return;
    }

    m_vulkanInstanceVoidHandle = static_cast<void*>(vulkanInstance);

    // Creating Surface
    VkSurfaceKHR surface = NULL;

#ifdef GLFW_INTERFACE

    GLFWwindow* glfwWindow;

    // What if not GLFW ? Well let's figure out then
    if (GLFWRenderWindow* render = dynamic_cast<GLFWRenderWindow*>(renderWindow))
    {
        glfwWindow = render->GetWindowPointer();
    }

    if (glfwCreateWindowSurface(vulkanInstance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
    {
        Console::LogError("failed to create window surface!");
        return;
    }
#else
    Console::LogError("Only GLFW build supported for Vulkan");
    return;
#endif

    m_vulkanSurfaceVoidHandle = reinterpret_cast<blaU64>(surface);

    // Skipping validation layer for now

    // Picking up physical device
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        Console::LogError("failed to find GPUs with Vulkan support!");
        return;
    }

    blaVector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());

    for (const auto& device : devices) 
    {
        if (BLAVulkanHelpers::IsSuitableDevice(device, surface))
        {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) 
    {
        Console::LogError("failed to find a suitable GPU!");
        return;
    }

    // Create Logical device
    VkDevice vulkanDevice;

    BLAVulkanHelpers::VkQueueFamilyIndices queueFamilies = BLAVulkanHelpers::FindQueueFamilies(physicalDevice, surface);
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { (uint32_t) queueFamilies.graphicsQueue, (uint32_t) queueFamilies.presentationQueue };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    // VkQueue a;
    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    //if (enableValidationLayers) {
    //    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    //    deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    //}
    //else {
        deviceCreateInfo.enabledLayerCount = 0;
    //}

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &vulkanDevice) != VK_SUCCESS) 
    {
        Console::LogError("failed to create logical device!");
        return;
    }

    VkQueue graphicsQueue, presentationQueue;

    vkGetDeviceQueue(vulkanDevice, queueFamilies.graphicsQueue, 0, &graphicsQueue);
    vkGetDeviceQueue(vulkanDevice, queueFamilies.presentationQueue, 0, &presentationQueue);

    m_vulkanGraphicsQueueVoidHandle = graphicsQueue;
    m_vulkanPresentationQueueVoidHandle = presentationQueue;

    // Create Swap chain
    BLAVulkanHelpers::SwapChainSupportDetails swapChainSupport = BLAVulkanHelpers::QuerySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = BLAVulkanHelpers::ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = BLAVulkanHelpers::ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = BLAVulkanHelpers::ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surface;

    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { (uint32_t)queueFamilies.graphicsQueue, (uint32_t)queueFamilies.presentationQueue };

    if (queueFamilies.graphicsQueue != queueFamilies.presentationQueue)
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else 
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;

    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain;

    if (vkCreateSwapchainKHR(vulkanDevice, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    m_swapChainVoidHandle = reinterpret_cast<blaU64>(swapChain);

    std::vector<VkImage> swapChainImages;

    vkGetSwapchainImagesKHR(vulkanDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vulkanDevice, swapChain, &imageCount, swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_extents2D.width = extent.width;
    m_extents2D.height = extent.height;
}

void VulkanRenderer::SwitchRenderingManager(RenderingManager* renderingManager) {}
bool VulkanRenderer::Update() { return true; }
int VulkanRenderer::SynchWithRenderManager() { return 0; }
RenderObject* VulkanRenderer::LoadRenderObject(const MeshRendererComponent& object, int type) { return nullptr; }
bool VulkanRenderer::CancelRender(const MeshRendererComponent& object) { return false; }
bool VulkanRenderer::LoadDebugLines() { return false; }
Ray VulkanRenderer::ScreenToRay(blaVec2 screenSpaceCoord) { return Ray(blaVec3(0), blaVec3(1), 10.f); }
void VulkanRenderer::CleanUpPools() {}
