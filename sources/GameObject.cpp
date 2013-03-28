#include "GameObject.h"


GameObject::GameObject(void)
{
	transform = new Transform();
	rigidBody = new RigidBody(transform);
	meshRenderer = new MeshRenderer(&(transform->transformMatrix));
}


GameObject::~GameObject(void)
{
}

void GameObject::Update()
{
	rigidBody->Update();
	transform->UpdateTransform();
}