#include "GameObject.h"


GameObject::GameObject(void)
{
	meshRenderer = new MeshRenderer();
}


GameObject::~GameObject(void)
{
}

void GameObject::SetPosition(vec3 position)
{
	this->transform.position = position;
	this->meshRenderer->meshTransform = this->transform.GetTransform();
}

void GameObject::SetScale(vec3 scale)
{
	this->transform.scale = scale;
	this->meshRenderer->meshTransform = this->transform.GetTransform();
}