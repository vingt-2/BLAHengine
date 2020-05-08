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

    VulkanState* vulkanState = new VulkanState();

    vulkanState->m_vulkanInstance = vulkanInstance;

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

    vulkanState->m_vulkanSurface = surface;

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

    BLAVulkanHelpers::QueueFamilyIndices queueFamilies = BLAVulkanHelpers::FindQueueFamilies(physicalDevice, surface);
    
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

    vulkanState->m_vulkanGraphicsQueue = graphicsQueue;
    vulkanState->m_vulkanPresentationQueue = presentationQueue;

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

    vulkanState->m_swapChain = swapChain;

    std::vector<VkImage> swapChainImages;

    vkGetSwapchainImagesKHR(vulkanDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vulkanDevice, swapChain, &imageCount, swapChainImages.data());

    vulkanState->m_swapChainImageFormat = surfaceFormat.format;
    vulkanState->m_extents2D.width = extent.width;
    vulkanState->m_extents2D.height = extent.height;

    vulkanState->m_swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) 
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = vulkanState->m_swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vulkanDevice, &createInfo, nullptr, &vulkanState->m_swapChainImageViews[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
	
    /*
	 * CreateRenderPass
	 */

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = vulkanState->m_swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(vulkanState->m_vulkanLogicalDevice, &renderPassInfo, nullptr, &vulkanState->m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }

    /*
     * Create Graphic Pipeline
     */
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = m_vulkanState->CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = m_vulkanState->CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vulkanState->m_extents2D.width;
    viewport.height = (float)vulkanState->m_extents2D.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = vulkanState->m_extents2D;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(vulkanState->m_vulkanLogicalDevice, &pipelineLayoutInfo, nullptr, &vulkanState->m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = vulkanState->m_pipelineLayout;
    pipelineInfo.renderPass = vulkanState->m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(vulkanState->m_vulkanLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vulkanState->m_graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(vulkanState->m_vulkanLogicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(vulkanState->m_vulkanLogicalDevice, vertShaderModule, nullptr);
	
	/*
	 * Create FrameBuffer
	 */

    for (size_t i = 0; i < vulkanState->m_swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = 
        {
            vulkanState->m_swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vulkanState->m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = vulkanState->m_extents2D.width;
        framebufferInfo.height = vulkanState->m_extents2D.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vulkanState->m_vulkanLogicalDevice, &framebufferInfo, nullptr, &vulkanState->m_swapChainFramebuffers[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }


    /*
     * Create Command Pool
     */
    
    BLAVulkanHelpers::QueueFamilyIndices queFamilyIndices = BLAVulkanHelpers::FindQueueFamilies(vulkanState->m_vulkanPhysicalDevice, vulkanState->m_vulkanSurface);

    VkCommandPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.queueFamilyIndex = queFamilyIndices.graphicsQueue;
    poolCreateInfo.flags = 0;

	if(vkCreateCommandPool(vulkanState->m_vulkanLogicalDevice, &poolCreateInfo, nullptr, &vulkanState->m_commandPool) != VK_SUCCESS)
	{
        throw std::runtime_error("Failed To create Command pool");
	}

	/*
	 * Create Command buffers
	 */

    vulkanState->m_commandBuffers.resize(vulkanState->m_swapChainFramebuffers.size());

	
	
    m_vulkanState = vulkanState;
}

void VulkanRenderer::SwitchRenderingManager(RenderingManager* renderingManager) {}
bool VulkanRenderer::Update() { return true; }
int VulkanRenderer::SynchWithRenderManager() { return 0; }
RenderObject* VulkanRenderer::LoadRenderObject(const MeshRendererComponent& object, int type) { return nullptr; }
bool VulkanRenderer::CancelRender(const MeshRendererComponent& object) { return false; }
bool VulkanRenderer::LoadDebugLines() { return false; }
Ray VulkanRenderer::ScreenToRay(blaVec2 screenSpaceCoord) { return Ray(blaVec3(0), blaVec3(1), 10.f); }
void VulkanRenderer::CleanUpPools() {}

void VulkanRenderer::CleanupRenderer()
{
	if(m_vulkanState)
	{
        vkDestroyPipelineLayout(m_vulkanState->m_vulkanLogicalDevice, m_vulkanState->m_pipelineLayout, nullptr);
        vkDestroyRenderPass(m_vulkanState->m_vulkanLogicalDevice, m_vulkanState->m_renderPass, nullptr);

        for (auto imageView : m_vulkanState->m_swapChainImageViews) {
            vkDestroyImageView(m_vulkanState->m_vulkanLogicalDevice, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_vulkanState->m_vulkanLogicalDevice, m_vulkanState->m_swapChain, nullptr);
        vkDestroyDevice(m_vulkanState->m_vulkanLogicalDevice, nullptr);

        //if (enableValidationLayers) {
        //    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        //}

        vkDestroySurfaceKHR(m_vulkanState->m_vulkanInstance, m_vulkanState->m_vulkanSurface, nullptr);
        vkDestroyInstance(m_vulkanState->m_vulkanInstance, nullptr);
    }
    delete m_vulkanState;
    m_vulkanState = nullptr;
}

