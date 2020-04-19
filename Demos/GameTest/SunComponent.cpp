#pragma once
#include <StdInclude.h>
#include <Maths/Maths.h>
#include <Animation/SkeletonAnimationData.h>
#include <Core/TransformComponent.h>
#include <Core/BehaviorComponent.h>

namespace BLAengine
{
    BeginBehaviorDeclaration(DemosLibrary, SunComponent)

        float m_speed = -0.0002f;

    EndBehaviorDeclaration()

    BeginBehaviorDescription(SunComponent, Dependencies(RootSystem))
        Expose(m_speed)
    EndBehaviorDescription()

    void SunComponent::Update()
    {
        TransformComponent* transformComp = GetOwnerObject().GetComponent<TransformComponent>();

        blaScaledTransform lightT = transformComp->GetTransform();

        lightT.SetEulerAngles(lightT.GetEulerAngles() + blaVec3(m_speed, 0.f, 0.f));

        transformComp->SetTransform(lightT);
    }
}
