#include "StdInclude.h"

#if NEW_VULKAN_RENDERER

#define GLFW_INCLUDE_VULKAN
#include "RenderBackend.h"

void check_vk_result(VkResult err);

namespace BLA
{
    struct VulkanFrameContext
    {
        VulkanFrameContext()
        {
            memset(this, 0, sizeof(*this));
        }

        VkCommandPool       CommandPool;
        VkCommandBuffer     CommandBuffer;
        VkFence             Fence;
        VkImage             Backbuffer;
        VkImageView         BackbufferView;
        VkFramebuffer       Framebuffer;
    };

    struct VulkanFrameSemaphore
    {
        VulkanFrameSemaphore()
        {
            memset(this, 0, sizeof(*this));
        }

        VkSemaphore         ImageAcquiredSemaphore;
        VkSemaphore         RenderCompleteSemaphore;
    };

    struct VulkanWindowInfo
    {
        VkExtent2D          Extent;
        VkSwapchainKHR      Swapchain;
        VkSurfaceKHR        Surface;
        VkSurfaceFormatKHR  SurfaceFormat;
        VkPresentModeKHR    PresentMode;
        VkRenderPass        ImGuiRenderPass; //TODO: Obviously move this where it belongs ...
        bool                ClearEnable;
        VkClearValue        ClearValue;
        uint32_t            FrameIndex;    
        uint32_t            ImageCount;    
        uint32_t            SemaphoreIndex;
        blaVector<VulkanFrameContext> Frames;
        blaVector<VulkanFrameSemaphore> FrameSemaphores;

        VulkanWindowInfo(VkSurfaceKHR surface)
        {
            memset(this, 0, sizeof(*this));
            PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
            ClearEnable = true;
            Surface = surface;
        }
    };

    struct VulkanContext
    {
        VkAllocationCallbacks*   m_Allocator = NULL;
        VkInstance               m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice         m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice                 m_Device = VK_NULL_HANDLE;
        uint32_t                 m_QueueFamily = (uint32_t)-1;
        VkQueue                  m_Queue = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT m_DebugReport = VK_NULL_HANDLE;
        VkPipelineCache          m_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool         m_DescriptorPool = VK_NULL_HANDLE;
        int                      m_MinImageCount = 2;
    };
};

#endif