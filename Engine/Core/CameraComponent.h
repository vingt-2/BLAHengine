#pragma once
#include "StdInclude.h"
#include "Maths/Maths.h"

#include "GameComponent.h"

namespace BLA
{
    BeginComponentDeclaration(BLAEngine, CameraComponent)
        void Init() override;

        blaScaledTransform  m_viewTransform;
        
        bool m_isShadowMapCamera = false;

        // Functions
        void UpdateView();
        void Update();

    EndComponentDeclaration()
}