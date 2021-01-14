// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "StdInclude.h"
#include "System.h"
#include "System/Vulkan/Context.h"
#include "Rendering/RenderPass.h"

#define VK_CHECK_RESULT(x) x

namespace BLA
{
    VkViewport MakeViewport(float width, float height, float minDepth, float maxDepth);

    VkRect2D MakeRect2D(int32_t width, int32_t height, int32_t offsetX, int32_t offsetY);

    struct VulkanRenderPassInstance
    {
        const BaseRenderPassInstance* m_renderPassInstancePtr;

        VkDescriptorSet m_descriptorSets;
    };

	namespace Gpu
	{
        struct RenderAttachment;
	}

    class VulkanRenderPass : public Gpu::RenderPassImplementation
    {
    protected:
        VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
        VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_vkPipeline = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_vkDescriptorSetLayout = VK_NULL_HANDLE;

        VkImageView m_attachmentImageView = VK_NULL_HANDLE;
        VkFramebuffer m_frameBuffer = VK_NULL_HANDLE; // framebuffer used to draw the instances for this vulkanrenderpass
        
        blaVec2 m_attachmentSize;
        blaVector<VulkanRenderPassInstance> m_currentInstances;
    
    public:

		void RegisterRenderPassInstance(const System::Vulkan::Context* vulkanInterface, const BaseRenderPassInstance& rpInstance);
		
        void SetAttachment(Gpu::RenderPassDescriptor& renderPassDescriptor, Gpu::RenderAttachment& attachment, VkDevice device);
		
        void CreatePipeline(
            Gpu::RenderPassDescriptor& renderPassDescriptor,
            VkDevice device,
            const VkAllocationCallbacks* allocator,
            VkPipelineCache pipelineCache,
            VkShaderModule vertexModule,
            VkShaderModule fragmentModule);

        void CreateVKRenderPass(VkDevice device);

        void BuildCommandBuffersThisFrame(const System::Vulkan::Context* vulkanContext) const;
    };
}
