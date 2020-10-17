// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/RenderingManager.h"
#include "Core/InspectableVariables.h"
#include "Renderer/ShadowRender.h"
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