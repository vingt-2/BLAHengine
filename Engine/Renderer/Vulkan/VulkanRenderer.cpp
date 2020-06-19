// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"
#include "Renderer/RenderPass.h"

#if NEW_VULKAN_RENDERER

#define GLFW_INCLUDE_VULKAN

#include "RenderBackend.h"
#include "System/Console.h"
#include "System/RenderWindow.h"
#include "Renderer/MeshRendererComponent.h"
#include "Renderer/PointLightComponent.h"

#include "System/VulkanRenderWindow.h"
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

VulkanRenderer::VulkanRenderer(const AssetManager* assetManager)
{}

void VulkanRenderer::InitializeRenderer(RenderWindow* renderWindow)
{
    m_renderWindow = dynamic_cast<GLFWVulkanRenderWindow*>(renderWindow);

    m_renderWindow->GetSize(m_viewPortExtents.x, m_viewPortExtents.y);


}

bool VulkanRenderer::Update()
{	
	return true;
}

void VulkanRenderer::CleanupRenderer()
{
}

void VulkanRenderer::CreateDisplayBuffers()
{
    //VkImageCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    //memset(&createInfo, 0, sizeof(createInfo));

    //createInfo.extent.width = m_viewPortExtents.x;
    //createInfo.extent.height = m_viewPortExtents.y;
    //createInfo.extent.depth = 1;
    //createInfo.imageType = VK_IMAGE_TYPE_2D;
    //createInfo.format = VK_FORMAT_R8G8B8_UNORM;
    //createInfo.mipLevels = 1;
    //createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    //createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    //// We will sample directly from the color attachment
    //createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    //VkMemoryAllocateInfo memAlloc{};
    //VkMemoryRequirements memReqs;
}

#endif
