#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>

#include "GameComponent.h"

namespace BLAengine
{
    class BLACORE_API CameraComponent : public GameComponent
    {
    public:
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

    private:
    };


}