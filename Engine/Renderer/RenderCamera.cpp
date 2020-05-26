// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RenderCamera.h"

using namespace BLA;

void RenderCamera::AttachCamera(CameraComponent* camera)
{
    this->m_attachedCamera = camera;
}

void PerspectiveCamera::SetPerspective(glm::vec2 renderSize)
{
    //m_perspectiveProjection = glm::perspective(fovY, aspect, 0.0f1f, 10000.f);
    m_perspectiveProjection = glm::frustum(-0.00001f*renderSize.x, 0.00001f*renderSize.x, -0.00001f*renderSize.y, 0.00001f*renderSize.y, 0.01f, 10000.0f);
}

void PerspectiveCamera::Update()
{
    this->m_attachedCamera->Update();

    blaMat4 cameraTransformMat;
    m_attachedCamera->m_viewTransform.GetScaledTransformMatrix(cameraTransformMat);

    if (m_attachedCamera != nullptr)
        m_ViewProjection = m_perspectiveProjection * cameraTransformMat;
    else
        m_ViewProjection = blaMat4(1);
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
    this->m_attachedCamera->Update();

    blaMat4 cameraTransformMat;
    m_attachedCamera->m_viewTransform.GetScaledTransformMatrix(cameraTransformMat);

    if (m_attachedCamera != nullptr)
        m_ViewProjection = m_orthographicProjection * cameraTransformMat;
    else
        m_ViewProjection = blaMat4(1);
}
