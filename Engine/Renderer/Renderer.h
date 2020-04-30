#pragma once

#include <Core/RenderingManager.h>
#include <System/RenderWindow.h>
#include <Maths/Ray.h>

#include "RenderCamera.h"

namespace BLA
{
    class DirectionalLightRender;
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

    class BLACORE_API Renderer
    {
    public:
        PerspectiveCamera m_mainRenderCamera;

        blaMap<blaU32, RenderObject*> m_meshRenderPool;

        blaMap<blaU32, DirectionalLightRender*> m_directionalLightPool;
        blaMap<blaU32, PointLightRender*> m_pointLightPool;

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

        //TODO: Remove, improve blaguiwindow design ...
        virtual void SetRenderSize(blaIVec2 renderSize);

        void SetCamera(CameraComponent* camera);

        Renderer();
        virtual ~Renderer(void) = default;

        void SetRenderToFrameBufferOnly(bool renderToFrameBuffer) { m_renderToFrameBufferOnly = renderToFrameBuffer; }

    protected:
        RenderWindow* m_renderWindow;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;

        blaIVec2 m_renderSize;
        bool m_isContextEnabled;
        bool m_isFullScreen;
        bool m_renderToFrameBufferOnly;
    };


}