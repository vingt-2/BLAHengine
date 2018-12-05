#include "RenderCamera.h"

using namespace BLAengine;

void RenderCamera::AttachCamera(CameraComponent* camera)
{
    this->m_attachedCamera = camera;
}

void PerspectiveCamera::SetPerspective(glm::vec2 renderSize)
{
    //m_perspectiveProjection = glm::perspective(fovY, aspect, 0.0f1f, 10000.f);
    m_perspectiveProjection = glm::frustum(-0.001f*renderSize.x, 0.001f*renderSize.x, -0.001f*renderSize.y, 0.001f*renderSize.y, 1.f, 10000.0f);
}

void PerspectiveCamera::Update()
{
    this->m_attachedCamera->Update();

    if (m_attachedCamera != nullptr)
        m_ViewProjection = m_perspectiveProjection * m_attachedCamera->m_viewTransform.GetScaledTransformMatrix();
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

    if (m_attachedCamera != nullptr)
        m_ViewProjection = m_orthographicProjection * m_attachedCamera->m_viewTransform.GetScaledTransformMatrix();
    else
        m_ViewProjection = blaMat4(1);
}
