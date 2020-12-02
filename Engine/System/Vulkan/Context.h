#pragma once

#include "StdInclude.h"
#include "System/GraphicsAdapter.h"

namespace BLA
{
    namespace System
    {
        namespace Vulkan
        {
            struct FrameContext
            {
                FrameContext()
                {
                    memset(this, 0, sizeof(*this));
                }

                VkCommandPool       m_commandPool;
                VkCommandBuffer     m_commandBuffer;
                VkFence             m_imageFence;
                VkImage             m_backbuffer;
                VkImageView         m_backBufferView;
                VkFramebuffer       m_framebuffer;
            };

            struct FrameSemaphore
            {
                FrameSemaphore()
                {
                    memset(this, 0, sizeof(*this));
                }

                VkSemaphore         m_imageAcquiredSemaphore;
                VkSemaphore         m_renderCompleteSemaphore;
            };

            struct WindowInfo
            {
                VkExtent2D          m_extent;
                VkSwapchainKHR      m_swapchain;
                VkSurfaceKHR        m_surface;
                VkSurfaceFormatKHR  m_surfaceFormat;
                VkPresentModeKHR    m_presentMode;
                VkRenderPass        m_renderWindowPresentationPass;
                bool                m_clearEnable;
                VkClearValue        m_clearValue;
                uint32_t            m_frameIndex;
                uint32_t            m_imageCount;
                uint32_t            m_semaphoreIndex;
                blaVector<FrameContext> m_frames;
                blaVector<FrameSemaphore> m_frameSemaphores;

                WindowInfo(VkSurfaceKHR surface)
                {
                    memset(this, 0, sizeof(*this));
                    m_presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
                    m_clearEnable = true;
                    m_surface = surface;
                }
            };

            class Context
            {
                VkCommandPool m_currentCommandPool;
                VkCommandPool m_TransferCommandPool;

            public:

                Context(const char** extensions, uint32_t extensions_count);

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

                static void HandleError(VkResult);
            };
        }
    }
}