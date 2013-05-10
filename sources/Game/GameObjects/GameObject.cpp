#include "../../Headers/GameObject.h"


GameObject::GameObject(void)
{
	transform = new Transform();
	objectName = string("");
}


GameObject::~GameObject(void)
{
	transform->~Transform();
}

void GameObject::Update()
{
	transform->UpdateTransform();
}