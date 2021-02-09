// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Vulkan.h"
#include "System/Vulkan/Context.h"

#define VK_CHECK_RESULT(x) x

#define VMA_IMPLEMENTATION

#define VMA_STATIC_VULKAN_FUNCTIONS 1
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

#include "vk_mem_alloc.h"

#include "Resource.h"
#include "StaticBuffer.h"
#include "Image.h"
#include "RenderPassAttachment.h"
#include "RenderPassDescription.h"
// TODO: This is problematic, whatever handles renderpasses should probably higher level, meaning I've got to abstract this part
// So that setting up a renderpass is not Vulkan Specific
#include "Texture.h"
#include "Opaques.h"
#include "Rendering/RenderPass.h"

#include "System/FileSystem/Files.h"

#pragma optimize("", off)

namespace BLA::Gpu
{
    VkFormat g_BlaFormatToVulkan[] =
    {
        VK_FORMAT_R4G4_UNORM_PACK8,
        VK_FORMAT_R4G4B4A4_UNORM_PACK16,
        VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        VK_FORMAT_R5G6B5_UNORM_PACK16,
        VK_FORMAT_B5G6R5_UNORM_PACK16,
        VK_FORMAT_R5G5B5A1_UNORM_PACK16,
        VK_FORMAT_B5G5R5A1_UNORM_PACK16,
        VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        VK_FORMAT_R8_UNORM,
        VK_FORMAT_R8_SNORM,
        VK_FORMAT_R8_USCALED,
        VK_FORMAT_R8_SSCALED,
        VK_FORMAT_R8_UINT,
        VK_FORMAT_R8_SINT,
        VK_FORMAT_R8_SRGB,
        VK_FORMAT_R8G8_UNORM,
        VK_FORMAT_R8G8_SNORM,
        VK_FORMAT_R8G8_USCALED,
        VK_FORMAT_R8G8_SSCALED,
        VK_FORMAT_R8G8_UINT,
        VK_FORMAT_R8G8_SINT,
        VK_FORMAT_R8G8_SRGB,
        VK_FORMAT_R8G8B8_UNORM,
        VK_FORMAT_R8G8B8_SNORM,
        VK_FORMAT_R8G8B8_USCALED,
        VK_FORMAT_R8G8B8_SSCALED,
        VK_FORMAT_R8G8B8_UINT,
        VK_FORMAT_R8G8B8_SINT,
        VK_FORMAT_R8G8B8_SRGB,
        VK_FORMAT_B8G8R8_UNORM,
        VK_FORMAT_B8G8R8_SNORM,
        VK_FORMAT_B8G8R8_USCALED,
        VK_FORMAT_B8G8R8_SSCALED,
        VK_FORMAT_B8G8R8_UINT,
        VK_FORMAT_B8G8R8_SINT,
        VK_FORMAT_B8G8R8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SNORM,
        VK_FORMAT_R8G8B8A8_USCALED,
        VK_FORMAT_R8G8B8A8_SSCALED,
        VK_FORMAT_R8G8B8A8_UINT,
        VK_FORMAT_R8G8B8A8_SINT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SNORM,
        VK_FORMAT_B8G8R8A8_USCALED,
        VK_FORMAT_B8G8R8A8_SSCALED,
        VK_FORMAT_B8G8R8A8_UINT,
        VK_FORMAT_B8G8R8A8_SINT,
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        VK_FORMAT_A8B8G8R8_USCALED_PACK32,
        VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_FORMAT_A2R10G10B10_SNORM_PACK32,
        VK_FORMAT_A2R10G10B10_USCALED_PACK32,
        VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
        VK_FORMAT_A2R10G10B10_UINT_PACK32,
        VK_FORMAT_A2R10G10B10_SINT_PACK32,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        VK_FORMAT_A2B10G10R10_SNORM_PACK32,
        VK_FORMAT_A2B10G10R10_USCALED_PACK32,
        VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        VK_FORMAT_A2B10G10R10_SINT_PACK32,
        VK_FORMAT_R16_UNORM,
        VK_FORMAT_R16_SNORM,
        VK_FORMAT_R16_USCALED,
        VK_FORMAT_R16_SSCALED,
        VK_FORMAT_R16_UINT,
        VK_FORMAT_R16_SINT,
        VK_FORMAT_R16_SFLOAT,
        VK_FORMAT_R16G16_UNORM,
        VK_FORMAT_R16G16_SNORM,
        VK_FORMAT_R16G16_USCALED,
        VK_FORMAT_R16G16_SSCALED,
        VK_FORMAT_R16G16_UINT,
        VK_FORMAT_R16G16_SINT,
        VK_FORMAT_R16G16_SFLOAT,
        VK_FORMAT_R16G16B16_UNORM,
        VK_FORMAT_R16G16B16_SNORM,
        VK_FORMAT_R16G16B16_USCALED,
        VK_FORMAT_R16G16B16_SSCALED,
        VK_FORMAT_R16G16B16_UINT,
        VK_FORMAT_R16G16B16_SINT,
        VK_FORMAT_R16G16B16_SFLOAT,
        VK_FORMAT_R16G16B16A16_UNORM,
        VK_FORMAT_R16G16B16A16_SNORM,
        VK_FORMAT_R16G16B16A16_USCALED,
        VK_FORMAT_R16G16B16A16_SSCALED,
        VK_FORMAT_R16G16B16A16_UINT,
        VK_FORMAT_R16G16B16A16_SINT,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_FORMAT_R32_UINT,
        VK_FORMAT_R32_SINT,
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_UINT,
        VK_FORMAT_R32G32_SINT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_UINT,
        VK_FORMAT_R32G32B32_SINT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_UINT,
        VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_R64_UINT,
        VK_FORMAT_R64_SINT,
        VK_FORMAT_R64_SFLOAT,
        VK_FORMAT_R64G64_UINT,
        VK_FORMAT_R64G64_SINT,
        VK_FORMAT_R64G64_SFLOAT,
        VK_FORMAT_R64G64B64_UINT,
        VK_FORMAT_R64G64B64_SINT,
        VK_FORMAT_R64G64B64_SFLOAT,
        VK_FORMAT_R64G64B64A64_UINT,
        VK_FORMAT_R64G64B64A64_SINT,
        VK_FORMAT_R64G64B64A64_SFLOAT,
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_X8_D24_UNORM_PACK32,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT
    };

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

    struct VulkanRenderPassObject
    {
        const BaseRenderPassObject* m_RenderPassObjectPtr;

        VkDescriptorSet m_descriptorSet;
    };

    struct Vulkan::VulkanImplementation
    {
        VulkanImplementation(const System::Vulkan::Context* context) : m_vulkanContext(context)
        {
            VmaAllocatorCreateInfo allocatorInfo = {};
            allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
            allocatorInfo.physicalDevice = m_vulkanContext->m_physicalDevice;
            allocatorInfo.device = m_vulkanContext->m_device;
            allocatorInfo.instance = m_vulkanContext->m_instance;

            vmaCreateAllocator(&allocatorInfo, &m_allocator);

            VkCommandPoolCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            info.queueFamilyIndex = m_vulkanContext->m_queueFamily;
            vkCreateCommandPool(m_vulkanContext->m_device, &info, nullptr, &m_commandPool);

            m_queue = m_vulkanContext->m_queue;
        }

        const System::Vulkan::Context* m_vulkanContext;

        VmaAllocator m_allocator;

        VkQueue m_queue;
        VkCommandPool m_commandPool;

        VkCommandBuffer BeginSingleTimeCommands() const;

        void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

        void TransitionImageLayout(VkImage image, Gpu::Formats::Enum::Index format, VkImageLayout oldLayout, VkImageLayout newLayout) const;

        void CopyBufferToImage(VkBuffer buffer, Gpu::Formats::Enum::Index format, VkImage image, uint32_t width, uint32_t height) const;

        void CreateBuffer(
            VmaMemoryUsage memoryUsage,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkBuffer& buffer, VmaAllocation& allocation) const;

        void CreateImage(blaIVec2 size, Gpu::Formats::Enum::Index format, VkImage& image, VmaAllocation& allocation) const;

        void CreateImageView(VkImage image, Gpu::Formats::Enum::Index format, VkImageView& imageView) const;

        void LoadShaderCode(blaVector<blaU8> shaderCodeBlob, VkShaderModule& shaderModule);
    };

    class VulkanRenderPass : public Gpu::RenderPassInstanceImplementation
    {
    protected:
        VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
        VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_vkPipeline = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_vkDescriptorSetLayout = VK_NULL_HANDLE;

        struct VulkanAttachment
        {
            blaVector<VkImageView> m_attachmentImageViews;
            VkFramebuffer m_frameBuffer = VK_NULL_HANDLE;

            blaIVec2 m_attachmentSize;
        } m_attachment;

        blaVector<VulkanRenderPassObject> m_currentInstances;

    public:

        void RegisterRenderPassObject(const System::Vulkan::Context* vulkanInterface, const BaseRenderPassObject& rpInstance)
        {
            m_currentInstances.push_back(VulkanRenderPassObject());

            VulkanRenderPassObject& vulkanRenderPassObject = m_currentInstances[m_currentInstances.size() - 1];
            vulkanRenderPassObject.m_RenderPassObjectPtr = &rpInstance;

            // Allocate the descriptor sets for all the uniform
            // std::vector<VkDescriptorSetLayout> layouts(vulkanWindoInfo->m_frames.size(), m_vkDescriptorSetLayout);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = vulkanInterface->m_descriptorPool;
            allocInfo.descriptorSetCount = 1; // ONLY 1 DESCRIPTOR SET (needs to change for several frames in flight)
            allocInfo.pSetLayouts = &m_vkDescriptorSetLayout;

            VkResult err = vkAllocateDescriptorSets(vulkanInterface->m_device, &allocInfo, &vulkanRenderPassObject.m_descriptorSet);

            blaVector<VkDescriptorBufferInfo> bufferInfos(rpInstance.m_uvCount);
            blaVector<VkWriteDescriptorSet> descriptorWrites(rpInstance.m_uvCount + rpInstance.m_opaqueCount);
            for (blaU32 i = 0; i < bufferInfos.size(); i++)
            {
                const Gpu::BaseDynamicBuffer* buffer;
                rpInstance.GetUniformBufferObjectBuffer(i, buffer);

                VkDescriptorBufferInfo& bufferInfo = bufferInfos[i];
                bufferInfo.buffer = static_cast<VkBuffer>(buffer->GetHandle().m_impl.pointer);
                bufferInfo.offset = 0;
                bufferInfo.range = VK_WHOLE_SIZE;

                VkWriteDescriptorSet& descriptorWrite = descriptorWrites[i];
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vulkanRenderPassObject.m_descriptorSet;
                descriptorWrite.dstBinding = static_cast<blaU32>(i);
                // the buffers are ordered in the order they should appear in the shader
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfo;
                descriptorWrite.pImageInfo = nullptr; // Optional
                descriptorWrite.pTexelBufferView = nullptr; // Optional
            }

            for (blaU32 i = (blaU32)bufferInfos.size(); i < descriptorWrites.size(); i++)
            {
                const Gpu::Opaque* opaque;
                rpInstance.GetOpaqueValue(i, opaque);

                switch(opaque->GetType())
                {
                    case Opaque::Type::Sampler:
                    {
                        const Gpu::Sampler* sampler = static_cast<const Sampler*>(opaque);
                        break;
                    }
                }
            }

            vkUpdateDescriptorSets(vulkanInterface->m_device, static_cast<blaU32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }

        void SetAttachment(const BaseRenderPassAttachment* attachment, VkDevice device)
        {
            m_attachment.m_attachmentImageViews = blaVector<VkImageView>(attachment->m_colorCount + (attachment->m_depthImage ? 1 : 0));
            for(blaSize i = 0; i < attachment->m_colorCount; i++)
            {
                BaseImage* image;
                attachment->GetColorAttachment((blaU32)i, image);

                m_attachment.m_attachmentSize = image->GetSize();

                VkImageViewCreateInfo imageViewCreateInfo = {};
                imageViewCreateInfo.image = static_cast<VkImage>(image->GetHandle().m_impl.pointer);
                imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                imageViewCreateInfo.format = g_BlaFormatToVulkan[image->GetFormat()];
                imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                imageViewCreateInfo.subresourceRange.levelCount = 1;
                imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                imageViewCreateInfo.subresourceRange.layerCount = 1;
                vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_attachment.m_attachmentImageViews[i]);
            }

            if(attachment->m_depthImage)
            {
                BaseImage* image = attachment->m_depthImage;

                VkImageViewCreateInfo imageViewCreateInfo = {};
                imageViewCreateInfo.image = static_cast<VkImage>(image->GetHandle().m_impl.pointer);
                imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                imageViewCreateInfo.format = g_BlaFormatToVulkan[image->GetFormat()];
                imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                imageViewCreateInfo.subresourceRange.levelCount = 1;
                imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                imageViewCreateInfo.subresourceRange.layerCount = 1;
                vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_attachment.m_attachmentImageViews[m_attachment.m_attachmentImageViews.size()-1]);
            }

            VkFramebufferCreateInfo frameBufferCreateInfo = {};
            frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frameBufferCreateInfo.renderPass = m_vkRenderPass;
            frameBufferCreateInfo.attachmentCount = (blaU32)m_attachment.m_attachmentImageViews.size();
            frameBufferCreateInfo.pAttachments = m_attachment.m_attachmentImageViews.data();
            frameBufferCreateInfo.width = m_attachment.m_attachmentSize.x;
            frameBufferCreateInfo.height = m_attachment.m_attachmentSize.y;
            frameBufferCreateInfo.layers = 1;

            vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &m_attachment.m_frameBuffer);
        }

        void CreatePipeline(
            const Gpu::RenderPassDescriptor& renderPassDescriptor,
            VkDevice device,
            const VkAllocationCallbacks* allocator,
            VkPipelineCache pipelineCache,
            VkShaderModule vertexModule,
            VkShaderModule fragmentModule)
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
            for (blaU32 i = 0; i < vaCount; i++)
            {
                Core::InspectableVariables::ExposedVarTypeDescriptor* vaType = renderPassDescriptor.m_vertexAttributesDescriptors[i];
                VkVertexInputBindingDescription& bindingDescriptor = bindingDescriptions[i];
                bindingDescriptor.stride = static_cast<blaU32>(vaType->size);
                bindingDescriptor.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                VkVertexInputAttributeDescription& vaDescriptor = vaDescriptions[i];
                vaDescriptor.location = i;
                vaDescriptor.binding = i;
                vaDescriptor.format = g_BlaFormatToVulkan[Gpu::Formats::g_fundamentalTypeToFormat[vaType->m_typeID]];
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

            // TODO
            VkPipelineMultisampleStateCreateInfo ms_info = {};
            ms_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            ms_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // TODO static_cast<VkSampleCountFlagBits>(VK_SAMPLE_COUNT_1_BIT << (attachment.m_sampleCount - 1));

            VkPipelineColorBlendAttachmentState color_attachment[1] = {};
            color_attachment[0].blendEnable = VK_TRUE;
            color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
            color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            color_attachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
            color_attachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

            VkPipelineDepthStencilStateCreateInfo depth_info = {};
            depth_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depth_info.depthTestEnable = VK_TRUE;
            depth_info.depthWriteEnable = VK_TRUE;
            depth_info.depthCompareOp = VK_COMPARE_OP_LESS;

            VkPipelineColorBlendStateCreateInfo blend_info = {};
            blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            blend_info.attachmentCount = 1;
            blend_info.pAttachments = color_attachment;

            VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
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

        void CreateVKRenderPass(VkDevice device, const RenderPassDescriptor* rpDescriptor)
        {
            blaVector<VkAttachmentDescription> attachments(rpDescriptor->m_attachmentDescription.m_colorAttachments.size() 
                + (rpDescriptor->m_attachmentDescription.m_depthAttachment != Formats::Enum::INVALID ? 1 : 0));

            blaVector<VkAttachmentReference> colorAttachmentsRefs(rpDescriptor->m_attachmentDescription.m_colorAttachments.size());

            for(size_t i = 0; i < colorAttachmentsRefs.size(); i++)
            {
                /*
                 * TODO: Mayw want rpDescriptor->m_attachmentDescription.m_colorAttachments to be more than a format
                 */
                Formats::Enum::Index formatIndex = rpDescriptor->m_attachmentDescription.m_colorAttachments[i];

                VkAttachmentDescription& colorAttachment = attachments[i];

                colorAttachment.format = g_BlaFormatToVulkan[formatIndex];
                colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

                VkAttachmentReference& colorAttachmentRef = colorAttachmentsRefs[i];

                colorAttachmentRef.attachment = static_cast<blaU32>(i);
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = static_cast<blaU32>(colorAttachmentsRefs.size());
            subpass.pColorAttachments = colorAttachmentsRefs.data();

            VkAttachmentReference depthAttachmentRef{};
            if(rpDescriptor->m_attachmentDescription.m_depthAttachment != Formats::Enum::INVALID)
            {
                VkAttachmentDescription& depthAttachment = attachments[colorAttachmentsRefs.size()];

                depthAttachment.format = g_BlaFormatToVulkan[rpDescriptor->m_attachmentDescription.m_depthAttachment];
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

                depthAttachmentRef.attachment = static_cast<blaU32>(rpDescriptor->m_attachmentDescription.m_colorAttachments.size());
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                subpass.pDepthStencilAttachment = &depthAttachmentRef;
            }

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = static_cast<blaU32>(attachments.size());
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;

            VkResult err = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_vkRenderPass);

            //VulkanRenderPassHelpers::check_vk_result(err);
        }

        void BuildCommandBuffersThisFrame(const System::Vulkan::Context* vulkanContext) const
        {
            System::Vulkan::WindowInfo* vkWindow = vulkanContext->m_window;

            VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
            cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmdBufferBeginInfo.pNext = NULL;

            VkClearValue clearValues[2];
            clearValues[0].color = { {0.0f, 1.0f, 1.0f, 1.0f} };
            clearValues[1].depthStencil = {1.0f, 0};

            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = m_vkRenderPass;
            renderPassBeginInfo.renderArea.offset.x = 0;
            renderPassBeginInfo.renderArea.offset.y = 0;
            renderPassBeginInfo.renderArea.extent.width = m_attachment.m_attachmentSize.x;
            renderPassBeginInfo.renderArea.extent.height = m_attachment.m_attachmentSize.y;
            renderPassBeginInfo.clearValueCount = 2;
            renderPassBeginInfo.pClearValues = clearValues;

            int i = vkWindow->m_frameIndex;

            vkWaitForFences(vulkanContext->m_device, 1, &vulkanContext->m_window->m_frames[i].m_imageFence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking

            vkResetFences(vulkanContext->m_device, 1, &vulkanContext->m_window->m_frames[i].m_imageFence);

            renderPassBeginInfo.framebuffer = m_attachment.m_frameBuffer;

            vkBeginCommandBuffer(vkWindow->m_frames[i].m_commandBuffer, &cmdBufferBeginInfo);

            vkCmdBeginRenderPass(vkWindow->m_frames[i].m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport = MakeViewport(m_attachment.m_attachmentSize.x * 1.f, m_attachment.m_attachmentSize.y * 1.f, 0.0f, 1.0f);

            vkCmdSetViewport(vkWindow->m_frames[i].m_commandBuffer, 0, 1, &viewport);

            VkRect2D scissor = MakeRect2D(vkWindow->m_extent.width, (blaS32)vkWindow->m_extent.height, 0, 0);
            vkCmdSetScissor(vkWindow->m_frames[i].m_commandBuffer, 0, 1, &scissor);

            vkCmdBindPipeline(vkWindow->m_frames[i].m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);

            for (const VulkanRenderPassObject& instance : m_currentInstances)
            {
                blaVector<VkBuffer> vertexBuffers(instance.m_RenderPassObjectPtr->m_vaCount);
                blaVector<VkDeviceSize> offsets(instance.m_RenderPassObjectPtr->m_vaCount);
                for (blaU16 j = 0; j < instance.m_RenderPassObjectPtr->m_vaCount; j++)
                {
                    const Gpu::BaseStaticBuffer* b;
                    instance.m_RenderPassObjectPtr->GetVertexAttributeBuffer(j, b);
                    vertexBuffers[j] = static_cast<VkBuffer>(b->GetHandle().m_impl.pointer);
                    offsets[j] = 0;
                }

                vkCmdBindVertexBuffers(vkWindow->m_frames[i].m_commandBuffer, 0, (blaU32)vertexBuffers.size(), vertexBuffers.data(), offsets.data());

                VkBuffer indexBuffer = static_cast<VkBuffer>(instance.m_RenderPassObjectPtr->m_indices->GetHandle().m_impl.pointer);
                vkCmdBindIndexBuffer(vkWindow->m_frames[i].m_commandBuffer, indexBuffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);

                vkCmdBindDescriptorSets(vkWindow->m_frames[i].m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_vkPipelineLayout, 0, 1, &instance.m_descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(vkWindow->m_frames[i].m_commandBuffer, instance.m_RenderPassObjectPtr->m_indices->GetLength(), 1, 0, 0, 0);
            }

            vkCmdEndRenderPass(vkWindow->m_frames[i].m_commandBuffer);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &vkWindow->m_frames[i].m_commandBuffer;

            vkEndCommandBuffer(vkWindow->m_frames[i].m_commandBuffer);

            vkQueueSubmit(vulkanContext->m_queue, 1, &end_info, vulkanContext->m_window->m_frames[i].m_imageFence);

            vkWaitForFences(vulkanContext->m_device, 1, &vulkanContext->m_window->m_frames[i].m_imageFence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
        }
    };

    Vulkan::Vulkan(const System::Vulkan::Context* context)
    {
        m_implementation = new VulkanImplementation(context);
    }

    ResourceHandle Vulkan::Submit(BaseResource* resource)
    {
        switch (resource->GetType())
        {
        case EResourceType::eStaticBuffer:
        {
            return SubmitStaticBuffer(static_cast<BaseStaticBuffer*>(resource));
        }
        case EResourceType::eImage:
        {
            return SubmitImage(static_cast<BaseImage*>(resource));
        }
        case EResourceType::eTexture:
        {
            return SubmitTexture(static_cast<BaseTexture*>(resource));
        }
        case EResourceType::eShaderProgram:
        {
            return SubmitShaderProgram(static_cast<ShaderProgram*>(resource));
        }
        case EResourceType::eEnd: break;
        default:;
        }

        BLA_TRAP(false);

        return ResourceHandle();
    }

    void Vulkan::Cancel(BaseResource* resource)
    {
        switch (resource->GetType())
        {
        case EResourceType::eStaticBuffer:
        {
            CancelStaticBuffer(static_cast<BaseStaticBuffer*>(resource));
            return;
        }
        case EResourceType::eImage:
        {
            CancelImage(static_cast<BaseImage*>(resource));
            return;
        }
        case EResourceType::eShaderProgram:
        {
            CancelShaderProgram(static_cast<ShaderProgram*>(resource));
            return;
        }
        case EResourceType::eEnd: break;
        default:;
        }

        BLA_TRAP(false);
    }

    void Vulkan::PrepareForStaging(BaseResource* resource)
    {
        switch (resource->GetType())
        {
        case EResourceType::eStaticBuffer:
        {
            BaseStaticBuffer* bufferResource = static_cast<BaseStaticBuffer*>(resource);

            VkBuffer& stagingBuffer = reinterpret_cast<VkBuffer&>(bufferResource->m_StagingData.pointers[0]);
            VmaAllocation& stagingAlloc = reinterpret_cast<VmaAllocation&>(bufferResource->m_StagingData.pointers[1]);

            blaU32 bufferSize = bufferResource->GetElementSize() * bufferResource->GetLength();

            m_implementation->CreateBuffer(
                VMA_MEMORY_USAGE_CPU_TO_GPU,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                stagingBuffer, stagingAlloc);

            void* pointer;
            vmaMapMemory(m_implementation->m_allocator, stagingAlloc, &pointer);
            Interface::SetBufferDataPointer(bufferResource, reinterpret_cast<blaU8*>(pointer));

            // memcpy(data, resource->GetData(), bufferSize);
            // vmaUnmapMemory(m_implementation->m_allocator, stagingAlloc);

        }
        case EResourceType::eEnd: break;
        default:;
        }
    }

    ResourceHandle Vulkan::PrepareDynamicBuffer(BaseResource* resource)
    {
        switch (resource->GetType())
        {
        case EResourceType::eDynamicBuffer:
        {
            BaseDynamicBuffer* bufferResource = static_cast<BaseDynamicBuffer*>(resource);

            VkBuffer hostVisibleBuffer;
            VmaAllocation& bufferAlloc = reinterpret_cast<VmaAllocation&>(bufferResource->m_allocationHandle.pointer);

            m_implementation->CreateBuffer(
                VMA_MEMORY_USAGE_CPU_TO_GPU,
                bufferResource->GetSize(),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                hostVisibleBuffer, bufferAlloc);

            void* pointer;
            vmaMapMemory(m_implementation->m_allocator, bufferAlloc, &pointer);
            SetBufferDataPointer(bufferResource, reinterpret_cast<blaU8*>(pointer));

            ResourceHandle retVal;
            retVal.m_impl.pointer = hostVisibleBuffer;

            return retVal;
            // memcpy(data, resource->GetData(), bufferSize);
            // vmaUnmapMemory(m_implementation->m_allocator, stagingAlloc);

        }
        case EResourceType::eEnd: break;
        default:;
        }

        return ResourceHandle();
    }

    RenderPassInstanceImplementation* Vulkan::SetupRenderPass(const RenderPassDescriptor* rpDescriptor, RenderPassProgram& program)
    {
        VulkanRenderPass* renderPass = new VulkanRenderPass();

        renderPass->CreateVKRenderPass(m_implementation->m_vulkanContext->m_device, rpDescriptor);

        renderPass->CreatePipeline(
            *rpDescriptor,
            m_implementation->m_vulkanContext->m_device,
            nullptr,
            m_implementation->m_vulkanContext->m_pipelineCache,
            static_cast<VkShaderModule>(program.m_shaders[0].GetHandle().m_impl.pointer),
            static_cast<VkShaderModule>(program.m_shaders[1].GetHandle().m_impl.pointer));

       return renderPass;
    }

    void Vulkan::AttachToRenderPass(RenderPassInstanceImplementation* rpImplementation, const BaseRenderPassAttachment* attachment)
    {
        VulkanRenderPass* renderPass = static_cast<VulkanRenderPass*>(rpImplementation);

        renderPass->SetAttachment(attachment, m_implementation->m_vulkanContext->m_device);
    }

    void Vulkan::Render(RenderPassInstanceImplementation* renderPassInstanceImplementation)
    {
        static_cast<VulkanRenderPass*>(renderPassInstanceImplementation)->BuildCommandBuffersThisFrame(m_implementation->m_vulkanContext);
    }

    void Vulkan::RegisterRenderPassObjectBase(RenderPassInstanceImplementation* renderPassInstanceImplementation, const BaseRenderPassObject& instance)
    {
        static_cast<VulkanRenderPass*>(renderPassInstanceImplementation)->RegisterRenderPassObject(m_implementation->m_vulkanContext, instance);
    }

    ResourceHandle Vulkan::SubmitStaticBuffer(BaseStaticBuffer* resource)
    {
        BLA_TRAP(resource->m_StagingData.pointers[0] != nullptr && resource->m_StagingData.pointers[1] != nullptr);

        VkBuffer deviceLocalResourceBuffer;

        blaU32 bufferSize = resource->GetElementSize() * resource->GetLength();

        VkBufferUsageFlagBits usageFlag;

        switch (resource->m_usage)
        {
        case BaseStaticBuffer::Usage::VertexBuffer:
            usageFlag = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case BaseStaticBuffer::Usage::IndexBuffer:
            usageFlag = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        default:
            usageFlag = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
            BLA_TRAP(false);
            break;
        }

        m_implementation->CreateBuffer(
            VMA_MEMORY_USAGE_GPU_ONLY,
            bufferSize,
            usageFlag | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            deviceLocalResourceBuffer, reinterpret_cast<VmaAllocation&>(resource->m_allocationHandle));

        // Blocking copy call from host visible memory to gpu local (Creates and wait on the command on this thread's queue)
        m_implementation->CopyBuffer(reinterpret_cast<VkBuffer>(resource->m_StagingData.pointers[0]), deviceLocalResourceBuffer, bufferSize);

        ResourceHandle retVal;
        retVal.m_impl.pointer = deviceLocalResourceBuffer;

        return retVal;
    }

    ResourceHandle Vulkan::SubmitImage(BaseImage* resource)
    {
        Gpu::Formats::Enum::Index format = resource->GetFormat();

        VkImage image;
        m_implementation->CreateImage(
            resource->GetSize(),
            format,
            image,
            reinterpret_cast<VmaAllocation&>(resource->m_allocationHandle));

        m_implementation->TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        m_implementation->CopyBufferToImage(
            static_cast<VkBuffer>(GetImageBuffer(resource)->m_StagingData.pointers[0]), // <---- Needs a bunch of helper function to extract api specific shit form there ...
            format,
            image,
            resource->GetSize().x, resource->GetSize().y);

        m_implementation->TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

        ResourceHandle retVal;
        retVal.m_impl.pointer = image;

        return retVal;
    }

    ResourceHandle Vulkan::SubmitTexture(BaseTexture* resource)
    {
        VkImageView imageView;

        VkImage image = static_cast<VkImage>(resource->GetImage()->GetHandle().m_impl.pointer);

        m_implementation->CreateImageView(image, resource->GetImage()->GetFormat(), imageView);

        ResourceHandle retVal;
        retVal.m_impl.pointer = imageView;

        return retVal;
    }

    ResourceHandle Vulkan::SubmitShaderProgram(ShaderProgram* program)
    {
        blaVector<blaU8> shaderBlob = ReadBlob(program->m_pathToBinaries);

        VkShaderModule shaderModule;
        m_implementation->LoadShaderCode(shaderBlob, shaderModule);

        ResourceHandle retVal;
        retVal.m_impl.pointer = shaderModule;

        return retVal;
    }

    void Vulkan::CancelStaticBuffer(BaseStaticBuffer* resource)
    {

    }

    void Vulkan::CancelImage(BaseImage* resource)
    {

    }

    void Vulkan::CancelShaderProgram(ShaderProgram* shaderProgram)
    {

    }

    /*
     *  The actual vulkan interface is here
     */

    VkCommandBuffer Vulkan::VulkanImplementation::BeginSingleTimeCommands() const
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_vulkanContext->m_device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void Vulkan::VulkanImplementation::EndSingleTimeCommands(VkCommandBuffer commandBuffer) const
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_queue);

        vkFreeCommandBuffers(m_vulkanContext->m_device, m_commandPool, 1, &commandBuffer);
    }

    void Vulkan::VulkanImplementation::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void Vulkan::VulkanImplementation::TransitionImageLayout(VkImage image, Gpu::Formats::Enum::Index format, VkImageLayout oldLayout, VkImageLayout newLayout) const
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        bool isDepth = format >= Gpu::Formats::Enum::D16_UNORM;

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)// && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        EndSingleTimeCommands(commandBuffer);
    }

    void Vulkan::VulkanImplementation::CopyBufferToImage(VkBuffer buffer, Gpu::Formats::Enum::Index format, VkImage image, uint32_t width, uint32_t height) const
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        bool isDepth = format >= Gpu::Formats::Enum::D16_UNORM;

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        EndSingleTimeCommands(commandBuffer);
    }

    void Vulkan::VulkanImplementation::CreateBuffer(
        VmaMemoryUsage memoryUsage,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkBuffer& buffer,
        VmaAllocation& allocation) const
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = memoryUsage;

        vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    }

    void Vulkan::VulkanImplementation::CreateImage(blaIVec2 size, Gpu::Formats::Enum::Index format, VkImage& image, VmaAllocation& allocation) const
    {
        // Is it a depth image ?
        bool isDepth = format >= Formats::Enum::D16_UNORM;
        
        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = g_BlaFormatToVulkan[format];
        imageCreateInfo.extent.width = size.x;
        imageCreateInfo.extent.height = size.y;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

        imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | 
            (isDepth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT; //TODO !!!!!! ????? !!!!!

        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(m_allocator, &imageCreateInfo, &allocInfo, &image, &allocation, nullptr);
    }

    void Vulkan::VulkanImplementation::CreateImageView(VkImage image, Gpu::Formats::Enum::Index format,
        VkImageView& imageView) const
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = g_BlaFormatToVulkan[format];;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; //TODO TODO ... Needs a specification
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(m_vulkanContext->m_device, &viewInfo, nullptr, &imageView);
    }

    void Vulkan::VulkanImplementation::LoadShaderCode(blaVector<blaU8> shaderCodeBlob, VkShaderModule& shaderModule)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCodeBlob.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCodeBlob.data());

        if (vkCreateShaderModule(m_vulkanContext->m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
    }
}
