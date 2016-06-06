#include "RenderCamera.h"

void RenderCamera::AttachCamera(Camera* camera)
{
	this->m_attachedCamera = camera;
}

void PerspectiveCamera::SetPerspective(vec2 renderSize)
{
	//m_perspectiveProjection = glm::perspective(fovY, aspect, 0.01f, 10000.f);
	m_perspectiveProjection = glm::frustum(-0.0001f*renderSize.x, 0.0001f*renderSize.x, -0.0001f*renderSize.y, 0.0001f*renderSize.y, 0.1f, 10000.0f);
}

void PerspectiveCamera::Update()
{
	if (m_attachedCamera != NULL)
		m_ViewProjection = m_perspectiveProjection * m_attachedCamera->m_viewTransform.m_transformMatrix;
	else
		m_ViewProjection = mat4(1);
}

RenderCamera::RenderCamera()
{
	this->m_attachedCamera = NULL;
}

void OrthographicCamera::SetOrthographicProj(float left, float right, float bottom, float top)
{
	m_orthographicProjection = glm::ortho<float>(left, right, bottom, top, -100, 10000);
}

void OrthographicCamera::Update()
{
	if (m_attachedCamera != NULL)
		m_ViewProjection = m_orthographicProjection * m_attachedCamera->m_viewTransform.m_transformMatrix;
	else
		m_ViewProjection = mat4(1);
}
