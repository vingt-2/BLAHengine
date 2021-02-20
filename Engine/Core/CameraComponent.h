// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Maths/Maths.h"
#include "Maths/Ray.h"
#include "BehaviorComponent.h"
#include "Rendering/RenderCamera.h"

namespace BLA
{
    BeginBehaviorDeclaration(BLAEngine, CameraComponent)
        void Init() override;

        blaScaledTransform  m_worldToCamera;
        
        bool m_isShadowMapCamera = false;

        blaF32 m_fov = 120.f;
        blaF32 m_nearClipPlane = 0.01f;
        blaF32 m_farClipPlane = 10000.f;

        BLACORE_API Ray ClipSpaceToWorldRay(blaVec2 clipSpaceCoord);
    
        // Functions
        void UpdateView();

        PerspectiveCamera m_camera;

    EndBehaviorDeclaration()
}