// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved
#pragma once

#include "Core/InspectableVariables.h"
#include "System/Console.h"
#include "Renderer/ShadowRender.h"
#include "Renderer/RenderPass.h"
#include "Maths/Ray.h"

#include "RenderCamera.h"

namespace BLA
{

    class RenderWindow;
    class GLFWOpenGLRenderWindow;
    class BLACORE_API Renderer
    {
    public:
        PerspectiveCamera m_mainRenderCamera;
        Renderer();

        // Rendering Related
        virtual void InitializeRenderer(RenderWindow* renderWindow) = 0;
        virtual bool Update() = 0;
        virtual int	 SynchWithRenderManager() = 0;

        bool GetStatus()        const { return m_isContextEnabled; }
        blaVec2 GetRenderSize()    const { return m_renderSize; }

        //TODO: Remove, improve DevGuiwindow design ...
        virtual void SetRenderSize(blaIVec2 renderSize);

        void SetCamera(CameraComponent* camera);

        virtual ~Renderer(void) = default;

        void SetRenderToFrameBufferOnly(bool renderToFrameBuffer) { m_renderToFrameBufferOnly = renderToFrameBuffer; }

    protected:
        GLFWOpenGLRenderWindow* m_renderWindow;

        blaIVec2 m_renderSize;
        bool m_isContextEnabled;
        bool m_isFullScreen;
        bool m_renderToFrameBufferOnly;
    };
}