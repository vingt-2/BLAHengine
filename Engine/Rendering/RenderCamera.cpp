// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RenderCamera.h"
#include "Core/CameraComponent.h"

using namespace BLA;

void RenderCamera::AttachCamera(CameraComponent* camera)
{
    this->m_attachedCamera = camera;
}

void PerspectiveCamera::SetAspect(blaVec2 renderSize)
{
    m_aspectRatio = renderSize.x / renderSize.y;
}

/*
 * Creates a right handed homogeneous matrix for perspective projection based on desired FOV angles
 */
static blaMat4 GetPerspective(blaF32 horizontalFovRad, blaF32 verticalFovRad, blaF32 nearClipPlane, blaF32 farClipPlane)
{
    return blaMat4(
        blaVec4(0.5f / tanf(0.5f * horizontalFovRad), 0.f, 0.f, 0.f),
        blaVec4(0.f, 0.5f / tanf(0.5f * verticalFovRad), 0.f, 0.f),
        blaVec4(0.f, 0.f, nearClipPlane / (nearClipPlane - farClipPlane), -1.f),
        blaVec4(0.0f, 0.0f, (farClipPlane * nearClipPlane) / (nearClipPlane - farClipPlane), 0.f)
    );
}

void PerspectiveCamera::Update()
{
    SetAspect(blaVec2(16, 9));
    
    blaF32 horFovRad = m_attachedCamera->m_fov * DEG_TO_RAD;
    blaF32 vertFovRad = horFovRad / m_aspectRatio;
    
    m_perspectiveProjection = GetPerspective(horFovRad, vertFovRad, m_attachedCamera->m_nearClipPlane, m_attachedCamera->m_farClipPlane);
    
    blaMat4 cameraTransformMat;
    m_attachedCamera->m_worldToCamera.GetScaledTransformMatrix(cameraTransformMat);

    if (m_attachedCamera != nullptr)
        m_worldToClipSpace = m_perspectiveProjection * cameraTransformMat;
    else
        m_worldToClipSpace = blaMat4(1);
}

blaVec3 PerspectiveCamera::ClipSpaceToCameraDirection(blaVec2 clipSpaceCoord)
{
    blaVec3 direction(1.f);
    direction.x = (2.0f * clipSpaceCoord.x - 1.f) / m_perspectiveProjection[0][0];
    direction.y = -(2.0f * clipSpaceCoord.y - 1.f) / m_perspectiveProjection[1][1];
    direction *= -1.f;
    SafeNormalize(direction);
    return direction;
}

RenderCamera::RenderCamera()
{
    this->m_attachedCamera = nullptr;
}

void OrthographicCamera::SetOrthographicProj(float left, float right, float bottom, float top)
{
    m_orthographicProjection = glm::ortho<float>(left, right, bottom, top, -100, 10000);
}

void OrthographicCamera::Update()
{
    blaMat4 cameraTransformMat;
    m_attachedCamera->m_worldToCamera.GetScaledTransformMatrix(cameraTransformMat);

    if (m_attachedCamera != nullptr)
        m_worldToClipSpace = m_orthographicProjection * cameraTransformMat;
    else
        m_worldToClipSpace = blaMat4(1);
}

blaVec3 OrthographicCamera::ClipSpaceToCameraDirection(blaVec2 clipSpaceCoord)
{
    return blaVec3(0.f);
}
