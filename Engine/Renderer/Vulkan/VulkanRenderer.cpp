// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"

#if VULKAN

#define GLFW_INCLUDE_VULKAN

#include "RenderBackend.h"
#include "System/Console.h"
#include "System/RenderWindow.h"
#include "Renderer/MeshRendererComponent.h"
#include "Renderer/PointLightComponent.h"

#include <optional>
#include "VulkanRenderer.h"
#include <set>

using namespace BLA;

const blaVector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

struct BLA::VulkanState
{
    VkInstance m_vulkanInstance;
    VkPhysicalDevice m_vulkanPhysicalDevice;

    VkExtent2D m_extents2D;

    VkDevice m_vulkanLogicalDevice;

    VkQueue m_vulkanGraphicsQueue;
    VkQueue m_vulkanPresentationQueue;

    VkSurfaceKHR m_vulkanSurface;
    VkSwapchainKHR m_swapChain;

    blaVector<VkImage> m_swapChainImages;
    blaVector<VkImageView> m_swapChainImageViews;

    VkFormat  m_swapChainImageFormat;

    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    blaVector<VkFramebuffer> m_swapChainFramebuffers;

    VkCommandPool m_commandPool;

    blaVector<VkCommandBuffer> m_commandBuffers;

    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
	
	VkShaderModule CreateShaderModule(const blaVector<char>& code)
	{
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_vulkanLogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
	}
};

class BLAVulkanHelpers 
{
public:
	
    struct QueueFamilyIndices
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

    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;

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

VulkanRenderer::VulkanRenderer(const AssetManager* assetManager) : Renderer(assetManager)
{}

void VulkanRenderer::InitializeRenderer(RenderWindow* renderWindow, RenderingManager* renderingManager, DebugRenderingManager* debugRenderManager)
{
    m_isContextEnabled = true;
}

void VulkanRenderer::SwitchRenderingManager(RenderingManager* renderingManager) {}

bool VulkanRenderer::Update()
{	
	return true;
}

int VulkanRenderer::SynchWithRenderManager() { return 0; }
RenderObject* VulkanRenderer::LoadRenderObject(const MeshRendererComponent& object, int type) { return nullptr; }
bool VulkanRenderer::CancelRender(const MeshRendererComponent& object) { return false; }
bool VulkanRenderer::LoadDebugLines() { return false; }
Ray VulkanRenderer::ScreenToRay(blaVec2 screenSpaceCoord) { return Ray(blaVec3(0), blaVec3(1), 10.f); }
void VulkanRenderer::CleanUpPools() {}

void VulkanRenderer::CleanupRenderer()
{
	//if(m_vulkanState)
	//{
 //       vkDestroyPipelineLayout(m_vulkanState->m_vulkanLogicalDevice, m_vulkanState->m_pipelineLayout, nullptr);
 //       vkDestroyRenderPass(m_vulkanState->m_vulkanLogicalDevice, m_vulkanState->m_renderPass, nullptr);

 //       for (auto imageView : m_vulkanState->m_swapChainImageViews) {
 //           vkDestroyImageView(m_vulkanState->m_vulkanLogicalDevice, imageView, nullptr);
 //       }

 //       vkDestroySwapchainKHR(m_vulkanState->m_vulkanLogicalDevice, m_vulkanState->m_swapChain, nullptr);
 //       vkDestroyDevice(m_vulkanState->m_vulkanLogicalDevice, nullptr);

 //       //if (enableValidationLayers) {
 //       //    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
 //       //}

 //       vkDestroySurfaceKHR(m_vulkanState->m_vulkanInstance, m_vulkanState->m_vulkanSurface, nullptr);
 //       vkDestroyInstance(m_vulkanState->m_vulkanInstance, nullptr);
 //   }
 //   delete m_vulkanState;
 //   m_vulkanState = nullptr;
}

#endif