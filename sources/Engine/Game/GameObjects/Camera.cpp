#include "Camera.h"


Camera::Camera():
	GameChar(),
	m_fovSize(vec2(1,1))
{
	m_rigidBody = new RigidBody(m_transform);
	UpdateView();

	m_isControlEnabled = false;
}

Camera::~Camera(void)
{
	GameObject::~GameObject();
	m_rigidBody->~RigidBody();
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
	GameChar::Update(); // Parent class update
	UpdateView(); // View Transform update
}
