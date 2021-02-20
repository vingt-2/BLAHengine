// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "CameraComponent.h"
#include "Core/GameObject.h"
#include "Core/TransformComponent.h"

using namespace BLA;

BeginBehaviorDescription(CameraComponent, Dependencies(RootSystem))
Expose(m_isShadowMapCamera)
Expose(m_fov)
Expose(m_nearClipPlane)
Expose(m_farClipPlane)
EndBehaviorDescription()

void CameraComponent::Init()
{
    UpdateView();
    m_camera.AttachCamera(this);
}

Ray CameraComponent::ClipSpaceToWorldRay(blaVec2 clipSpaceCoord)
{
    blaVec3 dirInCamera = m_camera.ClipSpaceToCameraDirection(clipSpaceCoord);

    blaPosQuat cameraToWorld = m_worldToCamera.GetPosQuat().GetInverse();

    return Ray(cameraToWorld.GetTranslation3(), cameraToWorld.GetRotation() * dirInCamera, MAX_NORMAL_FLOAT);
}

void CameraComponent::UpdateView()
{
    if (!GetOwnerObject().IsValid())
        return;

    blaScaledTransform t = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();

    m_worldToCamera.m_posQuat = t.m_posQuat.GetInverse();
}

void CameraComponent::Update()
{
    // Regular updates
    UpdateView(); // View TransformComponent update
    m_camera.Update();
}
