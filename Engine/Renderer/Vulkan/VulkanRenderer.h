// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/AssetsManager.h"
#include "Renderer/Renderer.h"

#include "RenderBackend.h"

namespace BLA
{
    struct TextureView
    {
        VkImage m_image;
        VkImageView m_imageView;
        VkDeviceMemory m_memory;
        bool m_updated;
    };

    struct ScreenFrameBuffer
    {
        
    };

    struct OffscreenRenderBuffer
    {
        TextureView m_color;
    };

    struct DirectionalShadowRender;
    class GLFWVulkanRenderWindow;
    class BLACORE_API VulkanRenderer
    {
        BLA_DECLARE_SINGLETON(VulkanRenderer)
    public:

        VulkanRenderer(const AssetManager* assetManager, const blaVector<blaU32>& rpIds);

        void InitializeRenderer(RenderWindow* renderWindow);

        bool Update();

        void CleanupRenderer();

        int GetStatus() { return 1; }

        void SetRenderToFrameBufferOnly(bool a) {};

        const GLFWVulkanRenderWindow* GetRenderWindow() const { return m_renderWindow; }

        void SetRenderSize(blaIVec2 renderSize);

        OffscreenRenderBuffer m_offscreenBuffer;
    private:

        void CreateDisplayBuffers();

        blaIVec2 m_viewPortExtents;
        GLFWVulkanRenderWindow* m_renderWindow;
    };

    class BLACORE_API ShaderResource
    {

    };

    template<class RenderPass>
    BLACORE_API blaString GenerateVulkanShaderTemplate();
}