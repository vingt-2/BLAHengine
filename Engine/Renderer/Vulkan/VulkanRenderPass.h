// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "StdInclude.h"
#include "System.h"
#include "System/Vulkan/VulkanInterface.h"

#define VK_CHECK_RESULT(x) x

namespace BLA
{
    class RenderPassInstanceBase;

	template<typename T>
	struct GetVulkanDataFormat
	{
		static constexpr VkFormat ms_dataFormat = VkFormat::VK_FORMAT_UNDEFINED;
	};

	VkViewport MakeViewport(float width, float height, float minDepth, float maxDepth);

	VkRect2D MakeRect2D(int32_t width, int32_t height, int32_t offsetX, int32_t offsetY);

	struct VulkanRenderPassInstance
	{
		RenderPassInstanceBase* m_renderPassInstancePtr;
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

	class BaseVulkanRenderPass
	{
	protected:
		VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
		VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_vkPipeline = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_vkDescriptorSetLayout = VK_NULL_HANDLE;
	};

 //   class VulkanRenderPass : BaseVulkanRenderPass
 //   {
	//	blaVector<VulkanRenderPassInstance> m_currentInstances;

 //       static void CreateIndexBuffer(const Vulkan::Interface* vulkanInterface, VkDevice device,
 //                                     blaVector<blaU32>& indices, VulkanRenderPassInstance& vulkanRenderPassInstance);

	//public:

	//	void RegisterRenderPassInstance(const Vulkan::WindowInfo* vulkanWindoInfo, const Vulkan::Interface* vulkanInterface, VkDevice device, const BaseRenderPassInstance& rpInstance)
	//	{
	//		m_currentInstances.push_back(VulkanRenderPassInstance());

	//		VulkanRenderPassInstance& vulkanRenderPassInstance = m_currentInstances[m_currentInstances.size() - 1];
	//		vulkanRenderPassInstance.m_renderPassInstancePtr = &rpInstance;

	//		// Create a Vertex Buffer for each Vertex Attribute
	//		// This will also copy the vertex data into resident memory
	//		/*VulkanRenderPassHelpers::CreateVertexBuffer<typename RenderPass::RenderPassInstance, RenderPass::ms_VACount, RenderPass::ms_VACount>::
	//	        Create(vulkanInterface, device, rpInstance, vulkanRenderPassInstance.m_vertexBuffers, vulkanRenderPassInstance.m_vertexBufferMemories);*/

	//		// Create the uniform buffers
	//		/*VulkanRenderPassHelpers::CreateUniformBuffers<typename RenderPass::RenderPassInstance, RenderPass::ms_UVCount, RenderPass::ms_UVCount>::
	//	        Create(static_cast<int>(vulkanWindoInfo->m_frames.size()), vulkanInterface, device, rpInstance, vulkanRenderPassInstance.m_uniformBuffers);*/

	//		// Create the index buffer
	//		CreateIndexBuffer(vulkanInterface, device, rpInstance, vulkanRenderPassInstance);

	//		// Allocate the descriptor sets for all the uniform
	//		std::vector<VkDescriptorSetLayout> layouts(vulkanWindoInfo->m_frames.size(), m_vkDescriptorSetLayout);
	//		VkDescriptorSetAllocateInfo allocInfo{};
	//		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	//		allocInfo.descriptorPool = vulkanInterface->m_descriptorPool;
	//		allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanWindoInfo->m_frames.size());
	//		allocInfo.pSetLayouts = layouts.data();

	//		vulkanRenderPassInstance.m_descriptorSets.resize(vulkanWindoInfo->m_frames.size());

	//		VkResult err = vkAllocateDescriptorSets(device, &allocInfo, vulkanRenderPassInstance.m_descriptorSets.data());

	//		for (blaSize i = 0; i < vulkanWindoInfo->m_frames.size(); i++)
	//		{
	//			blaVector<VkDescriptorBufferInfo> bufferInfos(vulkanRenderPassInstance.m_uniformBuffers.size());
	//			blaVector<VkWriteDescriptorSet> descriptorWrites(vulkanRenderPassInstance.m_uniformBuffers.size());
	//			for (blaSize j = 0; j < descriptorWrites.size(); j++)
	//			{
	//				VkDescriptorBufferInfo& bufferInfo = bufferInfos[j];
	//				bufferInfo.buffer = vulkanRenderPassInstance.m_uniformBuffers[j].bufferMemories[i].first;
	//				bufferInfo.offset = 0;
	//				bufferInfo.range = sizeof(vulkanRenderPassInstance.m_uniformBuffers[j].size);

	//				VkWriteDescriptorSet& descriptorWrite = descriptorWrites[j];
	//				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//				descriptorWrite.dstSet = vulkanRenderPassInstance.m_descriptorSets[i];
	//				descriptorWrite.dstBinding = static_cast<blaU32>(j); // the buffers are ordered in the order they should appear in the shader
	//				descriptorWrite.dstArrayElement = 0;
	//				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//				descriptorWrite.descriptorCount = 1;
	//				descriptorWrite.pBufferInfo = &bufferInfo;
	//				descriptorWrite.pImageInfo = nullptr; // Optional
	//				descriptorWrite.pTexelBufferView = nullptr; // Optional
	//			}

	//			vkUpdateDescriptorSets(device, static_cast<blaU32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	//		}
	//	}

	//    void CreatePipeline(
	//		VkDevice device,
	//		const VkAllocationCallbacks* allocator,
	//		VkPipelineCache pipelineCache,
	//		VkSampleCountFlagBits MSAASamples,
	//		VkShaderModule vertexModule,
	//		VkShaderModule fragmentModule)
	//	{
	//		VkPipelineShaderStageCreateInfo stage[2] = {};
	//		stage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	//		stage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	//		stage[0].module = vertexModule;
	//		stage[0].pName = "main";
	//		stage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	//		stage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	//		stage[1].module = fragmentModule;
	//		stage[1].pName = "main";

	//		/* !!!!! Please Read !!!!!
	//		 * This is only for non-interleaved data layout
	//		 * !!!!!!!!!!!!!!!!!!!!!!!
	//		*/
	//		// Setup the binding descriptor for non-interleaved layout
	//		VkVertexInputBindingDescription binding_desc[RenderPass::ms_VACount] = {};
	//		VulkanRenderPassHelpers::SetupVertexInputBindingDescription<typename RenderPass::RenderPassInstance, RenderPass::ms_VACount, RenderPass::ms_VACount>::Setup(&binding_desc[0]);

	//		// Setup the vertex attribute descriptor for non-interleaved layout
	//		VkVertexInputAttributeDescription attribute_desc[RenderPass::ms_VACount] = {};
	//		VulkanRenderPassHelpers::SetupVertexInputAttributeDescription<typename RenderPass::RenderPassInstance, RenderPass::ms_VACount, RenderPass::ms_VACount>::Setup(&attribute_desc[0], &binding_desc[0]);

	//		VkPipelineVertexInputStateCreateInfo vertex_info = {};
	//		vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	//		vertex_info.vertexBindingDescriptionCount = RenderPass::ms_VACount;
	//		vertex_info.pVertexBindingDescriptions = binding_desc;
	//		vertex_info.vertexAttributeDescriptionCount = RenderPass::ms_VACount;
	//		vertex_info.pVertexAttributeDescriptions = attribute_desc;

	//		VkPipelineInputAssemblyStateCreateInfo ia_info = {};
	//		ia_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	//		ia_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	//		VkPipelineViewportStateCreateInfo viewport_info = {};
	//		viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	//		viewport_info.viewportCount = 1;
	//		viewport_info.scissorCount = 1;

	//		VkPipelineRasterizationStateCreateInfo raster_info = {};
	//		raster_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	//		raster_info.polygonMode = VK_POLYGON_MODE_FILL;
	//		raster_info.cullMode = VK_CULL_MODE_NONE;
	//		raster_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	//		raster_info.lineWidth = 1.0f;

	//		VkPipelineMultisampleStateCreateInfo ms_info = {};
	//		ms_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	//		ms_info.rasterizationSamples = (MSAASamples != 0) ? MSAASamples : VK_SAMPLE_COUNT_1_BIT;

	//		VkPipelineColorBlendAttachmentState color_attachment[1] = {};
	//		color_attachment[0].blendEnable = VK_TRUE;
	//		color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//		color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//		color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
	//		color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//		color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//		color_attachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
	//		color_attachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	//		VkPipelineDepthStencilStateCreateInfo depth_info = {};
	//		depth_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	//		VkPipelineColorBlendStateCreateInfo blend_info = {};
	//		blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	//		blend_info.attachmentCount = 1;
	//		blend_info.pAttachments = color_attachment;

	//		VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	//		VkPipelineDynamicStateCreateInfo dynamic_state = {};
	//		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//		dynamic_state.dynamicStateCount = 2;
	//		dynamic_state.pDynamicStates = dynamic_states;

	//		VkDescriptorSetLayoutBinding binding[RenderPass::ms_UVCount] = {};
	//		VulkanRenderPassHelpers::SetupDescriptorSetLayoutBinding<typename RenderPass::RenderPassInstance, RenderPass::ms_UVCount, RenderPass::ms_UVCount>::Setup(&binding[0]);

	//		m_vkDescriptorSetLayout = VK_NULL_HANDLE;

	//		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	//		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	//		descriptorSetLayoutCreateInfo.bindingCount = RenderPass::ms_UVCount;
	//		descriptorSetLayoutCreateInfo.pBindings = binding;
	//		VkResult err = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, allocator, &m_vkDescriptorSetLayout);
	//		VulkanRenderPassHelpers::check_vk_result(err);

	//		VkDescriptorSetLayout set_layout[1] = { m_vkDescriptorSetLayout };
	//		VkPipelineLayoutCreateInfo layout_info = {};
	//		layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//		layout_info.setLayoutCount = 1;
	//		layout_info.pSetLayouts = set_layout;
	//		layout_info.pushConstantRangeCount = 0;

	//		err = vkCreatePipelineLayout(device, &layout_info, allocator, &m_vkPipelineLayout);

	//		VulkanRenderPassHelpers::check_vk_result(err);

	//		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	//		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//		pipelineCreateInfo.flags = 0x00; // TODO: WHAT IS DAT ?
	//		pipelineCreateInfo.stageCount = 2;
	//		pipelineCreateInfo.pStages = stage;
	//		pipelineCreateInfo.pVertexInputState = &vertex_info;
	//		pipelineCreateInfo.pInputAssemblyState = &ia_info;
	//		pipelineCreateInfo.pViewportState = &viewport_info;
	//		pipelineCreateInfo.pRasterizationState = &raster_info;
	//		pipelineCreateInfo.pMultisampleState = &ms_info;
	//		pipelineCreateInfo.pDepthStencilState = &depth_info;
	//		pipelineCreateInfo.pColorBlendState = &blend_info;
	//		pipelineCreateInfo.pDynamicState = &dynamic_state;
	//		pipelineCreateInfo.layout = m_vkPipelineLayout;
	//		pipelineCreateInfo.renderPass = renderPass;

	//		err = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, allocator, &m_vkPipeline);
	//		VulkanRenderPassHelpers::check_vk_result(err);
	//	}

	//	void BuildCommandBuffers(blaVector<VulkanRenderPassInstance>& renderPassInstance, Vulkan::WindowInfo* vkWindow) const
	//    {
	//		VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
	//		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//		cmdBufferBeginInfo.pNext = NULL;

	//		VkClearValue clearValues[2];
	//		clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 0.0f } };
	//		clearValues[1].depthStencil = { 1.0f, 0 };

	//		VkRenderPassBeginInfo renderPassBeginInfo;
	//		renderPassBeginInfo.renderPass = vkWindow->m_renderWindowPresentationPass;
	//		renderPassBeginInfo.renderArea.offset.x = 0;
	//		renderPassBeginInfo.renderArea.offset.y = 0;
	//		renderPassBeginInfo.renderArea.extent.width = vkWindow->m_extent.width;
	//		renderPassBeginInfo.renderArea.extent.height = vkWindow->m_extent.height;
	//		renderPassBeginInfo.clearValueCount = 2;
	//		renderPassBeginInfo.pClearValues = clearValues;

	//		for (int32_t i = 0; i < vkWindow->m_frames.size(); ++i)
	//		{
	//			renderPassBeginInfo.framebuffer = vkWindow->m_frames[i].m_framebuffer;

	//			vkBeginCommandBuffer(vkWindow->m_frames[i].m_commandBuffer, &cmdBufferBeginInfo);

	//			vkCmdBeginRenderPass(vkWindow->m_frames[i].m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	//			VkViewport viewport = MakeViewport((float)vkWindow->m_extent.width, (float)vkWindow->m_extent.height, 0.0f, 1.0f);
	//			vkCmdSetViewport(vkWindow->m_frames[i].m_commandBuffer, 0, 1, &viewport);

	//			VkRect2D scissor = MakeRect2D(vkWindow->m_extent.width, (float)vkWindow->m_extent.height, 0, 0);
	//			vkCmdSetScissor(vkWindow->m_frames[i].m_commandBuffer, 0, 1, &scissor);

	//			vkCmdBindPipeline(vkWindow->m_frames[i].m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);

	//			for(VulkanRenderPassInstance& instance : renderPassInstance)
	//			{
	//				vkCmdBindVertexBuffers(vkWindow->m_frames[i].m_commandBuffer, 0, instance.m_vertexBuffers.size(), instance.m_vertexBuffers.data(), nullptr);

	//				vkCmdBindIndexBuffer(vkWindow->m_frames[i].m_commandBuffer, instance.m_indexBuffer.first, 0, VkIndexType::VK_INDEX_TYPE_UINT32);

	//				vkCmdBindDescriptorSets(vkWindow->m_frames[i].m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipelineLayout, 0, 1, &instance.m_descriptorSets[i], 0, nullptr);

	//			    vkCmdDrawIndexed(vkWindow->m_frames[i].m_commandBuffer, instance.m_indexCount, 1, 0, 0, 0);
	//			}

	//			vkCmdEndRenderPass(vkWindow->m_frames[i].m_commandBuffer);

	//			vkEndCommandBuffer(vkWindow->m_frames[i].m_commandBuffer);
	//		}
	//    }
 //   };
}
