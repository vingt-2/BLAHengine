// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"

#include "System/Console.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "System/RenderWindow.h"
#include <set>
#include <optional>
#include <random>

using namespace BLA;

const blaVector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

VulkanRenderer::VulkanRenderer(GLFWVulkanRenderWindow* pRenderWindow) : m_renderWindow(pRenderWindow)
{
    m_renderWindow->GetSize(m_viewPortExtents.x, m_viewPortExtents.y);

    CreateDisplayBuffers();
}

bool VulkanRenderer::Update()
{
	return true;
}

void VulkanRenderer::SetViewportSize(blaIVec2 renderSize)
{
	if(renderSize.x != m_viewPortExtents.x || renderSize.y != m_viewPortExtents.y)
	{
		m_viewPortExtents = renderSize;
		CreateDisplayBuffers();
	}
}

void VulkanRenderer::CreateDisplayBuffers()
{
	m_offscreenBuffer.m_color.m_updated = true;

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageCreateInfo.extent.width = m_viewPortExtents.x;
    imageCreateInfo.extent.height = m_viewPortExtents.y;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkMemoryAllocateInfo memAlloc{};
    VkMemoryRequirements memReqs;

    VkDevice device = m_renderWindow->GetVulkanInterface()->m_device;

    VkResult r = VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &m_offscreenBuffer.m_color.m_image));
    if(r != VkResult::VK_SUCCESS)
    {
        return;
    }
    vkGetImageMemoryRequirements(device, m_offscreenBuffer.m_color.m_image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = m_renderWindow->GetVulkanInterface()->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT); // should be device local

    r = VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &m_offscreenBuffer.m_color.m_memory));
    if (r != VkResult::VK_SUCCESS)
    {
        return;
    }
    r = VK_CHECK_RESULT(vkBindImageMemory(device, m_offscreenBuffer.m_color.m_image, m_offscreenBuffer.m_color.m_memory, 0));
    if (r != VkResult::VK_SUCCESS)
    {
        return;
    }

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0xFF, 0xEF);

	blaVector<blaU32> dummydata(m_viewPortExtents.x * m_viewPortExtents.y, distrib(gen) << 8 | distrib(gen) << 16 | distrib(gen) << 24| 0xFF);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	m_renderWindow->GetVulkanInterface()->CreateBuffer(
		sizeof(blaU32) * dummydata.size(),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, sizeof(blaU32) * dummydata.size(), 0, &data);
	memcpy(data, dummydata.data(), static_cast<size_t>(sizeof(blaU32) * dummydata.size()));
	vkUnmapMemory(device, stagingBufferMemory);

	m_renderWindow->GetVulkanInterface()->CopyBufferToImage(stagingBuffer, m_offscreenBuffer.m_color.m_image, m_viewPortExtents.x, m_viewPortExtents.y);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_offscreenBuffer.m_color.m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    r = VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &m_offscreenBuffer.m_color.m_imageView));
    if (r != VkResult::VK_SUCCESS)
    {
        return;
    }
}

RenderWindow* VulkanRenderer::GetRenderWindow()
{
    return static_cast<RenderWindow*>(m_renderWindow);
}
