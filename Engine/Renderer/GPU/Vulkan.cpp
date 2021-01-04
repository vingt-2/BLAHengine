// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Vulkan.h"
#include "Renderer/GPU/VulkanRenderPass.h"
#include "System/Vulkan/Context.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

#include "Resource.h"
#include "StaticBuffer.h"
#include "Image.h"

#pragma optimize("", off)

namespace BLA::Gpu
{ 
    struct Vulkan::VulkanImplementation
    {
        VulkanImplementation(const System::Vulkan::Context* context) : m_vulkanContext(context)
        {
            VmaAllocatorCreateInfo allocatorInfo = {};
            allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
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

        void CreateDeviceOnlyBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkBuffer& buffer, VmaAllocation& allocation) const;

        void CreateStagingBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkBuffer& buffer, VmaAllocation& allocation) const;

        void CreateImage(blaIVec2 size, VkImage& image, VmaAllocation& allocation) const;
    };

    Vulkan::Vulkan(const System::Vulkan::Context* context)
    {
        m_implementation = new VulkanImplementation(context);
    }

    ResourceHandle Vulkan::Submit(BaseResource* resource)
    {
        switch(resource->GetType())
        {
        case EResourceType::eStaticBuffer:
        {
            return SubmitStaticBuffer(static_cast<BaseStaticBuffer*>(resource));
        }
        case EResourceType::eImage:
        {
            return SubmitImage(static_cast<Image*>(resource));
        }
        case EResourceType::eEnd: break;
        default: ;
        }

        BLA_TRAP(false);

        return ResourceHandle();
    }

    void Vulkan::Cancel(ResourceHandle handle)
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

            m_implementation->CreateStagingBuffer(
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

    RenderPassImplementation* Vulkan::SetupRenderPass(RenderPassDescriptor& renderPassDescriptor)
    {
        VulkanRenderPass* renderPass = new VulkanRenderPass();
        renderPass->CreateVKRenderPass(m_implementation->m_vulkanContext->m_device);

       /* renderPass->CreatePipeline(
            renderPassDescriptor, 
            m_implementation->m_vulkanContext->m_device, 
            nullptr, 
            m_implementation->m_vulkanContext->m_pipelineCache,
            VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, */

        return renderPass;
    }

    ResourceHandle Vulkan::SubmitStaticBuffer(BaseStaticBuffer* resource)
    {
        BLA_TRAP(resource->m_StagingData.pointers[0] != nullptr && resource->m_StagingData.pointers[1] != nullptr);

        VkBuffer deviceLocalResourceBuffer;

        blaU32 bufferSize = resource->GetElementSize() * resource->GetLength();

        m_implementation->CreateDeviceOnlyBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            deviceLocalResourceBuffer, reinterpret_cast<VmaAllocation&>(resource->m_allocationHandle));

        // Blocking copy call from host visible memory to gpu local (Creates and wait on the command on this thread's queue)
        m_implementation->CopyBuffer(reinterpret_cast<VkBuffer>(resource->m_StagingData.pointers[0]), deviceLocalResourceBuffer, bufferSize);

        ResourceHandle retVal;
        retVal.m_impl.pointer = deviceLocalResourceBuffer;

        return retVal;
    }

    ResourceHandle Vulkan::SubmitImage(Image* resource)
    {
        VkImage image;
        m_implementation->CreateImage(
            resource->GetSize(),
            image,
            reinterpret_cast<VmaAllocation&>(resource->m_allocationHandle));

        m_implementation->CopyBufferToImage(
            static_cast<VkBuffer>(GetImageBuffer(resource)->m_StagingData.pointers[0]), // <---- Needs a bunch of helper function to extract api specific shit form there ...
            image,
            resource->GetSize().x, resource->GetSize().y);

        ResourceHandle retVal;
        retVal.m_impl.pointer = image;

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
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
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

    // TODO: Factor out create buffer logic with the memtype parameter
    void Vulkan::VulkanImplementation::CreateDeviceOnlyBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkBuffer& buffer, VmaAllocation& allocation) const
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    }

    void Vulkan::VulkanImplementation::CreateStagingBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation) const
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

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
        imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(m_allocator, &imageCreateInfo, &allocInfo, &image, &allocation, nullptr);
    }
}
