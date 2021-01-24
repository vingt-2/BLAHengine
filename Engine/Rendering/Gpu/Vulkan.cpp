// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Vulkan.h"
#include "Rendering/GPU/VulkanRenderPass.h"
#include "System/Vulkan/Context.h"

#define VMA_IMPLEMENTATION

#define VMA_STATIC_VULKAN_FUNCTIONS 1
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

#include "vk_mem_alloc.h"

#include "Resource.h"
#include "StaticBuffer.h"
#include "Image.h"
#include "RenderAttachment.h"
#include "System/FileSystem/Files.h"

namespace BLA::Gpu
{
    blaVector<VkFormat> g_BlaFormatToVulkan =
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

        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;

        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

        void CreateBuffer(
            VmaMemoryUsage memoryUsage,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkBuffer& buffer, VmaAllocation& allocation) const;

        void CreateImage(blaIVec2 size, VkImage& image, VmaAllocation& allocation) const;

        void LoadShaderCode(blaVector<blaU8> shaderCodeBlob, VkShaderModule& shaderModule);
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

    void Vulkan::SetupRenderPass(RenderPassDescriptor& renderPassDescriptor, RenderPassProgram& program)
    {
        VulkanRenderPass* renderPass = new VulkanRenderPass();

        renderPass->CreateVKRenderPass(m_implementation->m_vulkanContext->m_device);

        renderPass->CreatePipeline(
            renderPassDescriptor,
            m_implementation->m_vulkanContext->m_device,
            nullptr,
            m_implementation->m_vulkanContext->m_pipelineCache,
            static_cast<VkShaderModule>(program.m_shaders[0].GetHandle().m_impl.pointer),
            static_cast<VkShaderModule>(program.m_shaders[1].GetHandle().m_impl.pointer));

        renderPassDescriptor.m_pToInstanceRenderPassDescriptorPointer = renderPass;
    }

    void Vulkan::AttachToRenderPass(RenderPassDescriptor& renderPassDescriptor, RenderAttachment& attachment)
    {
        VulkanRenderPass* renderPass = static_cast<VulkanRenderPass*>(renderPassDescriptor.m_pToInstanceRenderPassDescriptorPointer);

        renderPass->SetAttachment(
            renderPassDescriptor,
            attachment,
            m_implementation->m_vulkanContext->m_device);
    }

    void Vulkan::Render(RenderPassDescriptor& renderPassDescriptor)
    {
        static_cast<VulkanRenderPass*>(renderPassDescriptor.m_pToInstanceRenderPassDescriptorPointer)->BuildCommandBuffersThisFrame(m_implementation->m_vulkanContext);
    }

    void Vulkan::RegisterRenderPassInstanceBase(const RenderPassDescriptor& descriptor, const BaseRenderPassInstance& instance)
    {
        static_cast<VulkanRenderPass*>(descriptor.m_pToInstanceRenderPassDescriptorPointer)->RegisterRenderPassInstance(m_implementation->m_vulkanContext, instance);
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
        VkImage image;
        m_implementation->CreateImage(
            resource->GetSize(),
            image,
            reinterpret_cast<VmaAllocation&>(resource->m_allocationHandle));

        m_implementation->TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        m_implementation->CopyBufferToImage(
            static_cast<VkBuffer>(GetImageBuffer(resource)->m_StagingData.pointers[0]), // <---- Needs a bunch of helper function to extract api specific shit form there ...
            image,
            resource->GetSize().x, resource->GetSize().y);

        m_implementation->TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

        ResourceHandle retVal;
        retVal.m_impl.pointer = image;

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

    VkCommandBuffer Vulkan::Vulkan::VulkanImplementation::BeginSingleTimeCommands() const
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

    void Vulkan::Vulkan::VulkanImplementation::EndSingleTimeCommands(VkCommandBuffer commandBuffer) const
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

    void Vulkan::Vulkan::VulkanImplementation::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void Vulkan::Vulkan::VulkanImplementation::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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

    void Vulkan::Vulkan::VulkanImplementation::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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

    void Vulkan::VulkanImplementation::CreateImage(blaIVec2 size, VkImage& image, VmaAllocation& allocation) const
    {
        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageCreateInfo.extent.width = size.x;
        imageCreateInfo.extent.height = size.y;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(m_allocator, &imageCreateInfo, &allocInfo, &image, &allocation, nullptr);
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
