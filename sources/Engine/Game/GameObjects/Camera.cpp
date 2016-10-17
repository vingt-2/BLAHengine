#include "Camera.h"

using namespace BLAengine;

Camera::Camera():
	GameObject(),
	m_fovSize(vec2(1,1))
{
	UpdateView();

	m_isControlEnabled = false;
}

Camera::~Camera(void)
{
	GameObject::~GameObject();
}


void Camera::UpdateView()
{
	m_viewTransform.m_position = -m_transform->m_position;
	m_viewTransform.m_rotation = 1.f * m_transform->m_rotation;
	m_viewTransform.UpdateTransform();
	m_viewTransform.m_transformMatrix = inverse(m_viewTransform.m_transformMatrix);
}

void Camera::Update()
{
	// Regular updates
	this->UpdateTransform();
	UpdateView(); // View Transform update
}
