#include "GameObject.h"


GameObject::GameObject(void)
{
	m_transform = new Transform();
}


GameObject::~GameObject(void)
{
	m_transform->~Transform();
}

void GameObject::UpdateTransform()
{
	this->m_transform->UpdateTransform();
}