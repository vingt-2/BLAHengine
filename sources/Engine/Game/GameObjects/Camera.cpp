#include "Camera.h"


Camera::Camera():
	GameChar(NULL),
	m_projection(mat4(1))
{
	m_rigidBody = new RigidBody(m_transform, NULL);
	UpdateView();

	m_isControlEnabled = false;
}


Camera::~Camera(void)
{
	GameObject::~GameObject();
	m_rigidBody->~RigidBody();
}

void Camera::SetProjection(mat4 projection)
{
	this->m_projection = projection;
}

void Camera::UpdateView()
{
	m_viewTransform.position = -m_transform->position;
	m_viewTransform.rotation = 1.f * m_transform->rotation;
	m_viewTransform.UpdateTransform();
	m_viewTransform.transformMatrix = inverse(m_viewTransform.transformMatrix);
}

void Camera::Update()
{
	if(m_isControlEnabled)
	{
		//SimpleControls();
	}
	// Regular updates
	GameChar::Update(); // Parent class update
	UpdateView(); // View Transform update
}
