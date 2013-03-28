#include "GameObject.h"


GameObject::GameObject(void)
{
	meshRenderer = new MeshRenderer(&(transform.transformMatrix));
}


GameObject::~GameObject(void)
{
}

void GameObject::Update()
{
	transform.UpdateTransform();
}