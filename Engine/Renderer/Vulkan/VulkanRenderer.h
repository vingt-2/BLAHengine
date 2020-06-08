// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/AssetsManager.h"

#include "Renderer/Renderer.h"

namespace BLA
{
    struct DirectionalShadowRender;
    struct VulkanState;
    class BLACORE_API VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer(const AssetManager* assetManager);

        void InitializeRenderer(RenderWindow* renderWindow, RenderingManager* renderingManager, DebugRenderingManager* debugRenderManager) override;
        void SwitchRenderingManager(RenderingManager* renderingManager) override;
        bool Update() override;
        int SynchWithRenderManager() override;
        RenderObject* LoadRenderObject(const MeshRendererComponent& object, int type) override;
        bool CancelRender(const MeshRendererComponent& object) override;
        bool LoadDebugLines() override;
        Ray ScreenToRay(blaVec2 screenSpaceCoord) override;
        void CleanUpPools() override;
        void CleanupRenderer();
    private:
    	
        VulkanState* m_vulkanState = nullptr;
    };
}
