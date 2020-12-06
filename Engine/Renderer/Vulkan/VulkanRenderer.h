// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/AssetsManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/Gpu/Interface.h"
#include "Renderer/Gpu/Image.h"

namespace BLA
{
    struct OffscreenRenderTarget
    {
        Gpu::Resource<Gpu::Image> m_color;
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
        OffscreenRenderTarget m_offscreenBuffer;

    private:
        void CreateOrUpdateRenderTargets();
        GLFWVulkanRenderWindow* m_renderWindow;
    };
}