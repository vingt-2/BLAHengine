#pragma once
#include <StdInclude.h>
#include <Maths/Maths.h>
#include <Animation/SkeletonAnimationData.h>
#include <Core/TransformComponent.h>

namespace BLAengine
{
    BeginComponentDeclaration(DemosLibrary, SunComponent)

        void Update() override;

        float m_speed = -0.0002f;

    EndComponentDeclaration()

    BeginComponentDescription(SunComponent)
        Expose(m_speed)
    EndComponentDescription()

    void SunComponent::Update()
    {
        TransformComponent* transformComp = GetOwnerObject().GetComponent<TransformComponent>();

        blaScaledTransform lightT = transformComp->GetTransform();

        lightT.SetEulerAngles(lightT.GetEulerAngles() + blaVec3(m_speed, 0.f, 0.f));

        transformComp->SetTransform(lightT);
    }
}
