#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>
#include <Engine/Animation/SkeletonAnimationData.h>

#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(SunComponent)

        void Update() override;

        SunComponent(GameObjectReference parentObject) : GameComponent(parentObject) {}
        ~SunComponent(void) override {}

        float m_speed = -0.0002f;

    END_DECLARATION()

    BEGIN_COMPONENT_DESCRIPTION(SunComponent)
        EXPOSE(m_speed)
    END_DESCRIPTION()

    void SunComponent::Update()
    {
        ObjectTransform& lightT = GetObjectTransform();

        lightT.SetEulerAngles(lightT.GetEulerAngles() + blaVec3(m_speed, 0.f, 0.f));
    }
}
