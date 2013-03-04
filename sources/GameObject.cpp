#include "GameObject.h"


GameObject::GameObject(void)
{
	meshRenderer = new MeshRenderer(&(transform.modelTransform));
}


GameObject::~GameObject(void)
{
}

void GameObject::Update()
{
	transform.UpdateTransform();
}