// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/RenderingManager.h"
#include "Core/InspectableVariables.h"
#include "System/Console.h"
#include "Renderer/ShadowRender.h"
#include "Renderer/RenderPass.h"
#include "Maths/Ray.h"

#include "RenderCamera.h"

namespace BLA
{
    class AssetManager;
    class PointLightRender;
    class BLACORE_API RenderObject
    {
    public:
        virtual ~RenderObject() {};
    private:

    };

    class BLACORE_API DebugRenderObject
    {
    public:
        virtual ~DebugRenderObject() {};
    private:

    };

	struct DirectionalLightRender
	{
        DirectionalLightComponent* m_dirLightComponent;
        DirectionalShadowRender m_shadowRender;
	};

    class RenderPassManager
    {
    public:
        blaVector<blaU32> m_renderPasses;
        blaVector<RenderPassInstanceContainer*> m_renderPassInstanceContainers;

        RenderPassManager(const blaVector<blaU32>& rpIds);

        template<typename RenderPass>
        void AddRenderPassInstance(const typename RenderPass::RenderPassInstance& rp)
        {
            bool renderPassRegistered = false;
            for (blaU32 registered : m_renderPasses)
            {
                if (registered == RenderPass::ms_renderPassId)
                {
                    renderPassRegistered = true;
                    break;
                }
            }
            if (!renderPassRegistered)
            {
                Console::LogError("Trying to add an instance of an unregistered render pass");
            }

            // Select the RenderPassInstanceContainer ...
            for (auto c : m_renderPassInstanceContainers)
            {
                if (c->GetId() == RenderPass::ms_renderPassId)
                {
                    reinterpret_cast<TypedRenderPassInstanceContainer<RenderPass>*>(c)->AddInstance(rp);
                }
            }
        }

    };

    class RenderWindow;
    class GLFWOpenGLRenderWindow;
    class BLACORE_API Renderer
    {
    public:
        PerspectiveCamera m_mainRenderCamera;

        blaMap<blaIndex, RenderObject*> m_meshRenderPool;

        blaMap<blaIndex, DirectionalLightRender> m_directionalLightPool;
        blaMap<blaIndex, PointLightComponent*> m_pointLightPool;

        // Rendering Related
        virtual void InitializeRenderer(RenderWindow* renderWindow, RenderingManager* renderingManager, DebugRenderingManager* debugRenderManager) = 0;
        virtual void SwitchRenderingManager(RenderingManager* renderingManager) = 0;
        virtual bool Update() = 0;
        virtual int	 SynchWithRenderManager() = 0;

        // New Interface ...
        // New Interface ...
        virtual RenderPassManager* GetRenderPassManager() = 0;

        virtual RenderObject* LoadRenderObject(const MeshRendererComponent& object, int type) = 0;
        virtual bool    CancelRender(const MeshRendererComponent& object) = 0;
        virtual bool	LoadDebugLines() = 0;
        virtual Ray		ScreenToRay(blaVec2 screenSpaceCoord) = 0;

        virtual void CleanUpPools() = 0;

        bool GetStatus()        const { return m_isContextEnabled; }
        blaVec2 GetRenderSize()    const { return m_renderSize; }

        //TODO: Remove, improve DevGuiwindow design ...
        virtual void SetRenderSize(blaIVec2 renderSize);

        void SetCamera(CameraComponent* camera);

        Renderer(const AssetManager* assetManager);
        virtual ~Renderer(void) = default;

        void SetRenderToFrameBufferOnly(bool renderToFrameBuffer) { m_renderToFrameBufferOnly = renderToFrameBuffer; }

    protected:
        GLFWOpenGLRenderWindow* m_renderWindow;

        const AssetManager* m_assetManager;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;

        blaIVec2 m_renderSize;
        bool m_isContextEnabled;
        bool m_isFullScreen;
        bool m_renderToFrameBufferOnly;
    };
}