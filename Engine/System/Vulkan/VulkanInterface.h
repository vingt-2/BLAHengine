#pragma once

#include "RenderBackend.h"
namespace BLA
{
    class VulkanInterface
    {
        VkCommandPool m_currentCommandPool;
        VkCommandPool m_TransferCommandPool;

    public:

        VulkanInterface(const char** extensions, uint32_t extensions_count);

        VkAllocationCallbacks*   m_allocator = NULL;
        VkInstance               m_instance = VK_NULL_HANDLE;
        VkPhysicalDevice         m_physicalDevice = VK_NULL_HANDLE;
        VkDevice                 m_device = VK_NULL_HANDLE;
        uint32_t                 m_queueFamily = (uint32_t)-1;
        VkQueue                  m_queue = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT m_debugReport = VK_NULL_HANDLE;
        VkPipelineCache          m_pipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool         m_descriptorPool = VK_NULL_HANDLE;
        int                      m_minImageCount = 2;
        VkPhysicalDeviceMemoryProperties m_memoryProperties;

        uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) const;

        VkCommandBuffer BeginSingleTimeCommands();

        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        void SetCommandPool(VkCommandPool& pool) { m_currentCommandPool = pool; }

        void SetTransferCommandPool() { m_currentCommandPool = m_TransferCommandPool; }

        void CreateBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);

        void HandleError(VkResult) {};
    };
}