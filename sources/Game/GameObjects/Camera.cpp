#include "Camera.h"


Camera::Camera():
	m_projection(mat4(1))
{
	rigidBody = new RigidBody(transform);
	UpdateView();

	isControlEnabled = false;
}


Camera::~Camera(void)
{
	GameObject::~GameObject();
	rigidBody->~RigidBody();
}

void Camera::SetProjection(mat4 projection)
{
	this->m_projection = projection;
}

void Camera::UpdateView()
{
	m_viewTransform.position = -transform->position;
	m_viewTransform.rotation = 1.f * transform->rotation;
	m_viewTransform.UpdateTransform();
	m_viewTransform.transformMatrix = inverse(m_viewTransform.transformMatrix);
}

void Camera::Update()
{
	if(isControlEnabled)
	{
		//SimpleControls();
	}
	// Regular updates
	GameChar::Update(); // Parent class update
	UpdateView(); // View Transform update
}
