// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"
#include "Core/InspectableVariables.h"
#include "VulkanRenderPass.h"


#include "Image.h"
#include "RenderAttachment.h"
#include "System/GraphicsAdapter.h"

#define VK_CHECK_RESULT(x) x

#pragma optimize("", off)

using namespace BLA;

blaMap<blaStringId, VkFormat> g_typeToFormat =
{
    { BlaStringId("blaVec3"), VkFormat::VK_FORMAT_R32G32B32_SFLOAT },
    { BlaStringId("blaVec2"), VkFormat::VK_FORMAT_R32G32_SFLOAT },
    { BlaStringId("blaU32"), VkFormat::VK_FORMAT_B8G8R8A8_UINT }
};

blaVector<char> readFile(const std::string& filename)
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

VkViewport BLA::MakeViewport(
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

VkRect2D BLA::MakeRect2D(
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

void VulkanRenderPass::RegisterRenderPassInstance(const System::Vulkan::Context* vulkanInterface, 
                                                  const BaseRenderPassInstance& rpInstance)
{
    m_currentInstances.push_back(VulkanRenderPassInstance());

    VulkanRenderPassInstance& vulkanRenderPassInstance = m_currentInstances[m_currentInstances.size() - 1];
    vulkanRenderPassInstance.m_renderPassInstancePtr = &rpInstance;

    // Allocate the descriptor sets for all the uniform
    // std::vector<VkDescriptorSetLayout> layouts(vulkanWindoInfo->m_frames.size(), m_vkDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanInterface->m_descriptorPool;
    allocInfo.descriptorSetCount = 1; // ONLY 1 DESCRIPTOR SET (needs to change for several frames in flight)
    allocInfo.pSetLayouts = &m_vkDescriptorSetLayout;

    VkResult err = vkAllocateDescriptorSets(vulkanInterface->m_device, &allocInfo, &vulkanRenderPassInstance.m_descriptorSets);

    blaVector<VkDescriptorBufferInfo> bufferInfos(rpInstance.m_uvCount);
    blaVector<VkWriteDescriptorSet> descriptorWrites(rpInstance.m_uvCount);
    for (blaU32 i = 0; i < descriptorWrites.size(); i++)
    {
        const Gpu::BaseDynamicBuffer* buffer;
        rpInstance.GetUniformValueBuffer(i, buffer);
    	
        VkDescriptorBufferInfo& bufferInfo = bufferInfos[i];
        bufferInfo.buffer = static_cast<VkBuffer>(buffer->GetHandle().m_impl.pointer);
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet& descriptorWrite = descriptorWrites[i];
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = vulkanRenderPassInstance.m_descriptorSets;
        descriptorWrite.dstBinding = static_cast<blaU32>(i);
        // the buffers are ordered in the order they should appear in the shader
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional
    }

    vkUpdateDescriptorSets(vulkanInterface->m_device, static_cast<blaU32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void VulkanRenderPass::CreatePipeline(Gpu::RenderPassDescriptor& renderPassDescriptor, 
									  Gpu::RenderAttachment& attachment,
									  VkDevice device,
                                      const VkAllocationCallbacks* allocator, 
									  VkPipelineCache pipelineCache,
                                      VkShaderModule vertexModule, VkShaderModule fragmentModule)
{
    VkPipelineShaderStageCreateInfo stage[2] = {};
    stage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stage[0].module = vertexModule;
    stage[0].pName = "main";
    stage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stage[1].module = fragmentModule;
    stage[1].pName = "main";

    ///* !!!!! Please Read !!!!!
    // * This is only for non-interleaved data layout
    // * !!!!!!!!!!!!!!!!!!!!!!!
    //
    // Setup the binding descriptor for non-interleaved layout
    // Iterate over all the vertex attributes (types)

    blaU32 vaCount = static_cast<blaU32>(renderPassDescriptor.m_vertexAttributesDescriptors.size());
    blaVector<VkVertexInputBindingDescription> bindingDescriptions(vaCount);
    blaVector<VkVertexInputAttributeDescription> vaDescriptions(vaCount);
    for(blaU32 i = 0; i < vaCount; i++)
    {
        Core::InspectableVariables::ExposedVarTypeDescriptor* vaType = renderPassDescriptor.m_vertexAttributesDescriptors[i];
        VkVertexInputBindingDescription& bindingDescriptor = bindingDescriptions[i];
        bindingDescriptor.stride = static_cast<blaU32>(vaType->size);
        bindingDescriptor.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription& vaDescriptor = vaDescriptions[i];
        vaDescriptor.location = i;
        vaDescriptor.binding = i;
        vaDescriptor.format = g_typeToFormat[vaType->m_typeID];
        vaDescriptor.offset = 0; // NON-INTERLEAVED DATA LAYOUT !!
    }

    VkPipelineVertexInputStateCreateInfo vertex_info = {};
    vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_info.vertexBindingDescriptionCount = vaCount;
    vertex_info.pVertexBindingDescriptions = bindingDescriptions.data();
    vertex_info.vertexAttributeDescriptionCount = vaCount;
    vertex_info.pVertexAttributeDescriptions = vaDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo ia_info = {};
    ia_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewport_info = {};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo raster_info = {};
    raster_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster_info.polygonMode = VK_POLYGON_MODE_FILL;
    raster_info.cullMode = VK_CULL_MODE_NONE;
    raster_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    raster_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo ms_info = {};
    ms_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms_info.rasterizationSamples = static_cast<VkSampleCountFlagBits>(VK_SAMPLE_COUNT_1_BIT << (attachment.m_sampleCount - 1));

	// Ok let's make the framebuffer here ...

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.image = static_cast<VkImage>(attachment.m_image->GetHandle().m_impl.pointer);
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // TODO: customize asap
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_attachmentImageView);
	
    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = m_vkRenderPass;
    frameBufferCreateInfo.attachmentCount = 1;
    frameBufferCreateInfo.pAttachments = &m_attachmentImageView;
    frameBufferCreateInfo.width = attachment.m_image->GetSize().x;
    frameBufferCreateInfo.height = attachment.m_image->GetSize().y;
    frameBufferCreateInfo.layers = 1;
	
    vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &m_frameBuffer);

    VkPipelineColorBlendAttachmentState color_attachment[1] = {};
    color_attachment[0].blendEnable = VK_TRUE;
    color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
    color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_attachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
    color_attachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_info = {};
    depth_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    VkPipelineColorBlendStateCreateInfo blend_info = {};
    blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend_info.attachmentCount = 1;
    blend_info.pAttachments = color_attachment;

    VkDynamicState dynamic_states[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;

    blaU32 uvCount = static_cast<blaU32>(renderPassDescriptor.m_uniformValuesDescriptors.size());
    blaVector<VkDescriptorSetLayoutBinding> descSetLayoutBindings(uvCount);
    for (blaU32 i = 0; i < uvCount; i++)
    {
        Core::InspectableVariables::ExposedVarTypeDescriptor* uvType = renderPassDescriptor.m_uniformValuesDescriptors[i];
        VkDescriptorSetLayoutBinding& descSetLayoutBinding = descSetLayoutBindings[i];
        descSetLayoutBinding.binding = i;
        descSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // TODO: Support other types ...
        descSetLayoutBinding.descriptorCount = 1;
        descSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
        descSetLayoutBinding.pImmutableSamplers = nullptr; // TODO: if sampler, add sampler
    }

    m_vkDescriptorSetLayout = VK_NULL_HANDLE;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = uvCount;
    descriptorSetLayoutCreateInfo.pBindings = descSetLayoutBindings.data();
    VkResult err = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, allocator, &m_vkDescriptorSetLayout);
    // VulkanRenderPassHelpers::check_vk_result(err);

    VkDescriptorSetLayout set_layout[1] = { m_vkDescriptorSetLayout };
    VkPipelineLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 1;
    layout_info.pSetLayouts = set_layout;
    layout_info.pushConstantRangeCount = 0;

    err = vkCreatePipelineLayout(device, &layout_info, allocator, &m_vkPipelineLayout);

    // VulkanRenderPassHelpers::check_vk_result(err);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.flags = 0x00; // TODO: WHAT IS DAT ?
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = stage;
    pipelineCreateInfo.pVertexInputState = &vertex_info;
    pipelineCreateInfo.pInputAssemblyState = &ia_info;
    pipelineCreateInfo.pViewportState = &viewport_info;
    pipelineCreateInfo.pRasterizationState = &raster_info;
    pipelineCreateInfo.pMultisampleState = &ms_info;
    pipelineCreateInfo.pDepthStencilState = &depth_info;
    pipelineCreateInfo.pColorBlendState = &blend_info;
    pipelineCreateInfo.pDynamicState = &dynamic_state;
    pipelineCreateInfo.layout = m_vkPipelineLayout;
    pipelineCreateInfo.renderPass = m_vkRenderPass;

    err = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, allocator, &m_vkPipeline);
    //VulkanRenderPassHelpers::check_vk_result(err);
}

void VulkanRenderPass::CreateVKRenderPass(VkDevice device)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM; // TODO: customize asap
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

    VkResult err = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_vkRenderPass);

    //VulkanRenderPassHelpers::check_vk_result(err);
}

void VulkanRenderPass::BuildCommandBuffersThisFrame(const System::Vulkan::Context* vulkanContext) const
{
    System::Vulkan::WindowInfo* vkWindow = vulkanContext->m_window;
	
    VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufferBeginInfo.pNext = NULL;

    VkClearValue clearValues[2];
    clearValues[0].color = {{0.0f, 0.0f, 0.2f, 0.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_vkRenderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = 20;
    renderPassBeginInfo.renderArea.extent.height = 20;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    int i = vkWindow->m_frameIndex;
	
    renderPassBeginInfo.framebuffer = m_frameBuffer;

    vkBeginCommandBuffer(vkWindow->m_frames[i].m_commandBuffer, &cmdBufferBeginInfo);

    vkCmdBeginRenderPass(vkWindow->m_frames[i].m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = MakeViewport(static_cast<float>(vkWindow->m_extent.width), static_cast<float>(vkWindow->m_extent.height), 0.0f, 1.0f);
    
    vkCmdSetViewport(vkWindow->m_frames[i].m_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = MakeRect2D(vkWindow->m_extent.width, (blaS32)vkWindow->m_extent.height, 0, 0);
    vkCmdSetScissor(vkWindow->m_frames[i].m_commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(vkWindow->m_frames[i].m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);

    for (const VulkanRenderPassInstance& instance : m_currentInstances)
    {
        blaVector<VkBuffer> vertexBuffers(instance.m_renderPassInstancePtr->m_vaCount);
        blaVector<VkDeviceSize> offsets(instance.m_renderPassInstancePtr->m_vaCount);
        for (blaU16 j = 0; j < instance.m_renderPassInstancePtr->m_vaCount; j++)
        {
            const Gpu::BaseStaticBuffer* b;
            instance.m_renderPassInstancePtr->GetVertexAttributeBuffer(j, b);
            vertexBuffers[j] = static_cast<VkBuffer>(b->GetHandle().m_impl.pointer);
            offsets[j] = 0;
        }

        vkCmdBindVertexBuffers(vkWindow->m_frames[i].m_commandBuffer, 0, (blaU32)vertexBuffers.size(), vertexBuffers.data(), offsets.data());

        VkBuffer indexBuffer = static_cast<VkBuffer>(instance.m_renderPassInstancePtr->m_indices->GetHandle().m_impl.pointer);
        vkCmdBindIndexBuffer(vkWindow->m_frames[i].m_commandBuffer, indexBuffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(vkWindow->m_frames[i].m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vkPipelineLayout, 0, 1, &instance.m_descriptorSets, 0, nullptr);

        vkCmdDrawIndexed(vkWindow->m_frames[i].m_commandBuffer, instance.m_renderPassInstancePtr->m_indices->GetLength(), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(vkWindow->m_frames[i].m_commandBuffer);

    vkEndCommandBuffer(vkWindow->m_frames[i].m_commandBuffer);
}