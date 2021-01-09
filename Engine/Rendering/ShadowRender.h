// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "System.h"
#include "RenderCamera.h"

namespace BLA
{

    struct ShadowRender
    {
        virtual blaMat4 GetShadowViewProjection() = 0;
        virtual void Update() = 0;  // <-- m_shadowCamera.Update();
    };

    struct DirectionalShadowRender : ShadowRender
    {
    public:
        blaVec3 m_shadowDirection;

        //TODO: THIS IS OpenGL... THIS NEEDS MURDERED
        blaU32 m_depthTexture;
        blaU32 m_shadowPrgmID;
        blaU32 m_shadowBuffer;

        int m_bufferSize;

        blaMat4 GetShadowViewProjection();
        void Update();

        OrthographicCamera m_shadowCamera;
    };

    struct PerspectiveShadowRender : ShadowRender
    {
    public:
        blaVec3 m_shadowDirection;

        //TODO: THIS IS OpenGL... THIS NEEDS MURDERED
        blaU32 m_depthTexture;
        blaU32 m_shadowPrgmID;
        blaU32 m_shadowBuffer;

        int m_bufferSize;

        blaMat4 GetShadowViewProjection();
        void Update();

        OrthographicCamera m_shadowCamera;

    };

    struct Cubemap : ShadowRender
    {
    public:
        blaVec3 m_shadowDirection;


        //TODO: THIS IS OpenGL... THIS NEEDS MURDERED
        blaU32 m_depthTexture[6];
        blaU32 m_shadowPrgmID;
        blaU32 m_shadowBuffers[6];

        int m_bufferSize;

        blaMat4 GetShadowViewProjection();
        void Update();

        PerspectiveCamera m_shadowCamera;
    };


}