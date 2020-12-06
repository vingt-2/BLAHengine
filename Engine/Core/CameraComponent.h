// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Maths/Maths.h"

#include "GameComponent.h"

namespace BLA
{
    BeginComponentDeclaration(BLAEngine, CameraComponent)
        void Init() override;

        blaScaledTransform  m_worldToCamera;
        
        bool m_isShadowMapCamera = false;

        blaF32 m_fov = 120.f;
        blaF32 m_nearClipPlane = 0.01f;
        blaF32 m_farClipPlane = 10000.f;
    
        // Functions
        void UpdateView();
        void Update();

    EndComponentDeclaration()
}