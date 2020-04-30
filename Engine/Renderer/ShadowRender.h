#pragma once
#include <System.h>
#include <RenderBackend.h>

#include "RenderCamera.h"

namespace BLA
{

    class BLACORE_API ShadowRender
    {
    public:
        virtual blaMat4 getShadowViewProjection() = 0;
        virtual void Update() = 0;  // <-- m_shadowCamera.Update();
    };

    class BLACORE_API DirectionalShadowRender : ShadowRender
    {
    public:
        bool m_isSetup;
        blaVec3 m_shadowDirection;

        //OpenGL
        GLuint m_depthTexture;
        GLuint m_shadowPrgmID;
        GLuint m_shadowBuffer;

        int m_bufferSize;

        blaMat4 getShadowViewProjection();
        void Update();

        OrthographicCamera m_shadowCamera;
    };

    class BLACORE_API PerspectiveShadowRender : ShadowRender
    {
    public:
        bool m_isSetup;
        blaVec3 m_shadowDirection;

        //OpenGL
        GLuint m_depthTexture;
        GLuint m_shadowPrgmID;
        GLuint m_shadowBuffer;

        int m_bufferSize;

        blaMat4 getShadowViewProjection();
        void Update();

        OrthographicCamera m_shadowCamera;

    };


}