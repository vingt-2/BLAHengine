#include "../Headers/Camera.h"


Camera::Camera():
	projection(mat4(1))
{
	rigidBody = new RigidBody(transform);
	UpdateView();
}


Camera::~Camera(void)
{
	GameObject::~GameObject();
	rigidBody->~RigidBody();
}

void Camera::SetProjection(mat4 projection)
{
	this->projection = projection;
}

void Camera::UpdateView()
{
	viewTransform.position = -transform->position;
	viewTransform.rotation = 1.f * transform->rotation;
	viewTransform.UpdateTransform();
	viewTransform.transformMatrix = inverse(viewTransform.transformMatrix);
}

void Camera::Update()
{
	GameObject::Update();
	rigidBody->Update();
	UpdateView();
}