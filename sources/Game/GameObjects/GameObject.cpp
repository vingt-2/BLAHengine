#include "GameObject.h"


GameObject::GameObject(void)
{
	transform = new Transform();
}


GameObject::~GameObject(void)
{
	transform->~Transform();
}

void GameObject::UpdateTransform()
{
	this->transform->UpdateTransform();
}