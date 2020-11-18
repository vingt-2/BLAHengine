// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "StdInclude.h"

#include "VulkanInterface.h"

void check_vk_result(VkResult err);

namespace BLA
{
    struct VulkanFrameContext
    {
        VulkanFrameContext()
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

    struct VulkanFrameSemaphore
    {
        VulkanFrameSemaphore()
        {
            memset(this, 0, sizeof(*this));
        }

        VkSemaphore         m_imageAcquiredSemaphore;
        VkSemaphore         m_renderCompleteSemaphore;
    };

    struct VulkanWindowInfo
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
        blaVector<VulkanFrameContext> m_frames;
        blaVector<VulkanFrameSemaphore> m_frameSemaphores;

        VulkanWindowInfo(VkSurfaceKHR surface)
        {
            memset(this, 0, sizeof(*this));
            m_presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
            m_clearEnable = true;
            m_surface = surface;
        }
    };
};