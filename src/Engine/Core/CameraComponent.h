#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>

#include "GameComponent.h"

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(CameraComponent)
        // Members
        ObjectTransform  m_viewTransform;
        glm::vec2        m_fovSize;

        // States
        bool m_isControlEnabled;
        
        bool m_isShadowMapCamera;

        // Functions
        void UpdateView();
        void Update();

        CameraComponent(GameObjectReference parentObject);
        ~CameraComponent(void) override;
    END_DECLARATION()
}