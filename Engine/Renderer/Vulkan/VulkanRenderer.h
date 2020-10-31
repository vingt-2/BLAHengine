// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/AssetsManager.h"
#include "Renderer/Renderer.h"

#define GLFW_INCLUDE_VULKAN
#include "RenderBackend.h"

namespace BLA
{
    struct TextureView;

    struct ScreenFrameBuffer
    {
        
    };

    struct OffscreenRenderBuffer
    {
        TextureView* m_color;
    };

    struct DirectionalShadowRender;
    class GLFWVulkanRenderWindow;
    class BLACORE_API VulkanRenderer
    {
        BLA_DECLARE_SINGLETON(VulkanRenderer)
    public:

        VulkanRenderer(const AssetManager* assetManager, const blaVector<blaU32>& rpIds);

        void InitializeRenderer(RenderWindow* renderWindow);

        RenderPassManager* GetRenderPassManager();
        bool Update();

        void CleanupRenderer();

        int GetStatus() { return 1; }

        void SetRenderToFrameBufferOnly(bool a) {};

    private:

        void CreateDisplayBuffers();

        OffscreenRenderBuffer m_offscreenBuffer;

        blaIVec2 m_viewPortExtents;
        GLFWVulkanRenderWindow* m_renderWindow;

        RenderPassManager m_renderPassManager;
    };

    class BLACORE_API ShaderResource
    {

    };

    template<class RenderPass>
    BLACORE_API blaString GenerateVulkanShaderTemplate();
}