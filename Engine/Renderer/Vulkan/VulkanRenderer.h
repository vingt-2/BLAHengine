// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/AssetsManager.h"
#include "Renderer/Renderer.h"
#include "System/Vulkan/VulkanInterface.h"

namespace BLA
{
    struct TextureView
    {
        VkImage m_image;
        VkImageView m_imageView;
        VkDeviceMemory m_memory;
        bool m_updated;
    };

    struct OffscreenRenderBuffer
    {
        TextureView m_color;
    };

    struct DirectionalShadowRender;
    class GLFWVulkanRenderWindow;
    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer(GLFWVulkanRenderWindow* pRenderWindow);

        bool Update() override;
        void SetViewportSize(blaIVec2 viewportSize) override;
        RenderWindow* GetRenderWindow() override;
        OffscreenRenderBuffer m_offscreenBuffer;

    private:
        void CreateDisplayBuffers();
        GLFWVulkanRenderWindow* m_renderWindow;
    };
}