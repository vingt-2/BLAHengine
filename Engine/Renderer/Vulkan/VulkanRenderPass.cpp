// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"
#include "VulkanRenderPass.h"

#if NEW_VULKAN_RENDERER

#define GLFW_INCLUDE_VULKAN

#include "RenderBackend.h"
#include "System/VulkanRenderWindow.h"
#include "VulkanRenderer.h"
#include <set>

#define VK_CHECK_RESULT(x) x

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

VkViewport MakeViewport(
	float width,
	float height,
	float minDepth,
	float maxDepth)
{
	VkViewport viewport = {};
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = minDepth;
	viewport.maxDepth = maxDepth;
	return viewport;
}

VkRect2D MakeRect2D(
	int32_t width,
	int32_t height,
	int32_t offsetX,
	int32_t offsetY)
{
	VkRect2D rect2D = {};
	rect2D.extent.width = width;
	rect2D.extent.height = height;
	rect2D.offset.x = offsetX;
	rect2D.offset.y = offsetY;
	return rect2D;
}

template<>
struct GetVulkanInputAttributeFormat<blaVec3>
{
	static constexpr VkFormat ms_dataFormat = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
};

template<>
struct GetVulkanInputAttributeFormat<blaVec2>
{
	static constexpr VkFormat ms_dataFormat = VkFormat::VK_FORMAT_R32G32_SFLOAT;
};

#endif
void VulkanRenderPassHelpers::check_vk_result(VkResult err)
{
	if (err == 0) return;
	printf("VkResult %d\n", err);
	if (err < 0)
		abort();
}

uint32_t VulkanRenderPassHelpers::FindMemoryType(const VulkanContext* context, uint32_t typeBits, VkMemoryPropertyFlags flags)
{
	return context->GetMemoryType(typeBits, flags);
}
