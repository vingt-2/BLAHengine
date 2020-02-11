#pragma once
#include <StdInclude.h>
#include <Maths/Maths.h>
#include <Animation/SkeletonAnimationData.h>
#include <Core/TransformComponent.h>

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(DemosLibrary, SunComponent)

        void Update() override;

        float m_speed = -0.0002f;

    END_DECLARATION()

    BEGIN_COMPONENT_DESCRIPTION(SunComponent)
        EXPOSE(m_speed)
    END_DESCRIPTION()

    void SunComponent::Update()
    {
        TransformComponent* transformComp = GetOwnerObject().GetComponent<TransformComponent>();

        blaScaledTransform lightT = transformComp->GetTransform();

        lightT.SetEulerAngles(lightT.GetEulerAngles() + blaVec3(m_speed, 0.f, 0.f));

        transformComp->SetTransform(lightT);
    }
}
