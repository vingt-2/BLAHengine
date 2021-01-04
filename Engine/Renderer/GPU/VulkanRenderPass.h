// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "StdInclude.h"
#include "System.h"
#include "System/Vulkan/Context.h"
#include "Renderer/RenderPass.h"

#define VK_CHECK_RESULT(x) x

namespace BLA
{
    VkViewport MakeViewport(float width, float height, float minDepth, float maxDepth);

    VkRect2D MakeRect2D(int32_t width, int32_t height, int32_t offsetX, int32_t offsetY);

    struct VulkanRenderPassInstance
    {
        const BaseRenderPassInstance* m_renderPassInstancePtr;
        typedef std::pair<VkBuffer, VkDeviceMemory> VKBufferAndMemory;

        struct VKUniformBufferEntry
        {
            const void* data;
            int size;
            blaVector<VKBufferAndMemory> bufferMemories;
        };

        blaVector<VkBuffer> m_vertexBuffers;
        blaVector<VkDeviceMemory> m_vertexBufferMemories;

        blaVector<VKUniformBufferEntry> m_uniformBuffers;
        blaVector<VkDescriptorSet> m_descriptorSets;

        blaU32 m_indexCount;
        VKBufferAndMemory m_indexBuffer;

        void UpdateUniformBuffersOnDevice(VkDevice device, int frameIndex);
    };

    class VulkanRenderPass : public Gpu::RenderPassImplementation
    {
        protected:
        VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
        VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_vkPipeline = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_vkDescriptorSetLayout = VK_NULL_HANDLE;
        blaVector<VulkanRenderPassInstance> m_currentInstances;

        static void CreateIndexBuffer(const System::Vulkan::Context* vulkanInterface, VkDevice device,
                                      blaVector<blaU32>& indices, VulkanRenderPassInstance& vulkanRenderPassInstance);

    public:

    void RegisterRenderPassInstance(const System::Vulkan::WindowInfo* vulkanWindoInfo,
                                    const System::Vulkan::Context* vulkanInterface, VkDevice device,
                                    const BaseRenderPassInstance& rpInstance);

        void CreatePipeline(
            Gpu::RenderPassDescriptor& renderPassDescriptor,
            VkDevice device,
            const VkAllocationCallbacks* allocator,
            VkPipelineCache pipelineCache,
            VkSampleCountFlagBits MSAASamples,
            VkShaderModule vertexModule,
            VkShaderModule fragmentModule);

        void CreateVKRenderPass(VkDevice device);

        void BuildCommandBuffers(blaVector<VulkanRenderPassInstance>& renderPassInstance,
                                 System::Vulkan::WindowInfo* vkWindow) const;
    };
}
