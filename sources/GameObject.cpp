#include "GameObject.h"


GameObject::GameObject(void)
{
	transform = new Transform();
}


GameObject::~GameObject(void)
{
}

void GameObject::Update()
{
	transform->UpdateTransform();
}