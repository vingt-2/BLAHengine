// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"
#include "VulkanRenderPass.h"
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
struct GetVulkanDataFormat<blaVec3>
{
    static constexpr VkFormat ms_dataFormat = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
};

template<>
struct GetVulkanDataFormat<blaVec2>
{
    static constexpr VkFormat ms_dataFormat = VkFormat::VK_FORMAT_R32G32_SFLOAT;
};

void VulkanRenderPassInstance::UpdateUniformBuffersOnDevice(VkDevice device, int frameIndex)
{
    for (auto& uniformBuffer : m_uniformBuffers)
    {
        void* data;
        vkMapMemory(device, uniformBuffer.bufferMemories[frameIndex].second, 0, uniformBuffer.size, 0, &data);
        // We are never using this memory mapping to write on data, it is therefore safe

        memcpy(data, uniformBuffer.data, uniformBuffer.size);
        vkUnmapMemory(device, uniformBuffer.bufferMemories[frameIndex].second);
    }
}

void VulkanRenderPass::RegisterRenderPassInstance(const System::Vulkan::WindowInfo* vulkanWindoInfo,
                                                  const System::Vulkan::Context* vulkanInterface, VkDevice device,
                                                  const BaseRenderPassInstance& rpInstance)
{
    m_currentInstances.push_back(VulkanRenderPassInstance());

    VulkanRenderPassInstance& vulkanRenderPassInstance = m_currentInstances[m_currentInstances.size() - 1];
    vulkanRenderPassInstance.m_renderPassInstancePtr = &rpInstance;

    // Create a Vertex Buffer for each Vertex Attribute
    // This will also copy the vertex data into resident memory
    /*VulkanRenderPassHelpers::CreateVertexBuffer<typename RenderPass::RenderPassInstance, RenderPass::ms_VACount, RenderPass::ms_VACount>::
        Create(vulkanInterface, device, rpInstance, vulkanRenderPassInstance.m_vertexBuffers, vulkanRenderPassInstance.m_vertexBufferMemories);*/

    // Create the uniform buffers
    /*VulkanRenderPassHelpers::CreateUniformBuffers<typename RenderPass::RenderPassInstance, RenderPass::ms_UVCount, RenderPass::ms_UVCount>::
        Create(static_cast<int>(vulkanWindoInfo->m_frames.size()), vulkanInterface, device, rpInstance, vulkanRenderPassInstance.m_uniformBuffers);*/

    // Create the index buffer
    // CreateIndexBuffer(vulkanInterface, device, rpInstance, vulkanRenderPassInstance);

    // Allocate the descriptor sets for all the uniform
    std::vector<VkDescriptorSetLayout> layouts(vulkanWindoInfo->m_frames.size(), m_vkDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanInterface->m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanWindoInfo->m_frames.size());
    allocInfo.pSetLayouts = layouts.data();

    vulkanRenderPassInstance.m_descriptorSets.resize(vulkanWindoInfo->m_frames.size());

    VkResult err = vkAllocateDescriptorSets(device, &allocInfo, vulkanRenderPassInstance.m_descriptorSets.data());

    for (blaSize i = 0; i < vulkanWindoInfo->m_frames.size(); i++)
    {
        blaVector<VkDescriptorBufferInfo> bufferInfos(vulkanRenderPassInstance.m_uniformBuffers.size());
        blaVector<VkWriteDescriptorSet> descriptorWrites(vulkanRenderPassInstance.m_uniformBuffers.size());
        for (blaSize j = 0; j < descriptorWrites.size(); j++)
        {
            VkDescriptorBufferInfo& bufferInfo = bufferInfos[j];
            bufferInfo.buffer = vulkanRenderPassInstance.m_uniformBuffers[j].bufferMemories[i].first;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(vulkanRenderPassInstance.m_uniformBuffers[j].size);

            VkWriteDescriptorSet& descriptorWrite = descriptorWrites[j];
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = vulkanRenderPassInstance.m_descriptorSets[i];
            descriptorWrite.dstBinding = static_cast<blaU32>(j);
            // the buffers are ordered in the order they should appear in the shader
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional
        }

        vkUpdateDescriptorSets(device, static_cast<blaU32>(descriptorWrites.size()), descriptorWrites.data(), 0,
                               nullptr);
    }
}

//void VulkanRenderPass::CreateIndexBuffer(const Vulkan::Context* vulkanInterface, VkDevice device,
//                                         blaVector<blaU32>& indices, VulkanRenderPassInstance& vulkanRenderPassInstance)
//{
//    vulkanRenderPassInstance.m_indexCount = static_cast<blaU32>(indices.size());
//
//    VkBuffer& indexBuffer = vulkanRenderPassInstance.m_indexBuffer.first;
//
//    VkBufferCreateInfo bufferInfo{};
//    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferInfo.size = sizeof(blaU32) * vulkanRenderPassInstance.m_indexCount;
//    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
//    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//    VkResult err = vkCreateBuffer(device, &bufferInfo, nullptr, &indexBuffer);
//
//    VkMemoryRequirements memRequirements;
//    vkGetBufferMemoryRequirements(device, indexBuffer, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    allocInfo.allocationSize = memRequirements.size;
//    allocInfo.memoryTypeIndex = vulkanInterface->GetMemoryType(memRequirements.memoryTypeBits,
//                                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//    VkDeviceMemory& memory = vulkanRenderPassInstance.m_indexBuffer.second;
//
//    err = vkAllocateMemory(device, &allocInfo, nullptr, &memory);
//
//    void* data;
//    vkMapMemory(device, memory, 0, bufferInfo.size, 0, &data);
//    memcpy(data, indices.data(), (size_t)bufferInfo.size);
//    vkUnmapMemory(device, memory);
//}
