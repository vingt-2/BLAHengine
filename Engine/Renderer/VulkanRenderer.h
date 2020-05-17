#pragma once
#include "StdInclude.h"
#include "Assets/AssetsManager.h"

#include "Renderer.h"

namespace BLA
{
    class DirectionalShadowRender;
    struct VulkanState;
    class BLACORE_API VulkanRenderer : public Renderer
    {
    public:
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
