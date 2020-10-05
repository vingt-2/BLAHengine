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

    class RenderPassManager
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
    public:
        VulkanRenderer(const AssetManager* assetManager);

        void InitializeRenderer(RenderWindow* renderWindow);

        bool Update();

        void CleanupRenderer();

        int GetStatus() { return 1; }

        void SetRenderToFrameBufferOnly(bool a) {};

    private:

        void CreateDisplayBuffers();

        OffscreenRenderBuffer m_offscreenBuffer;

        blaIVec2 m_viewPortExtents;
        GLFWVulkanRenderWindow* m_renderWindow;
    };

    class BLACORE_API ShaderResource
    {

    };

    template<class RenderPass>
    BLACORE_API blaString GenerateVulkanShaderTemplate();
}