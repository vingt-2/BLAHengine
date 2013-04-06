#include "GameChar.h"


GameChar::GameChar(void)
{
	rigidBody		= new RigidBody(transform);
	meshRenderer	= new MeshRenderer(transform);
}


GameChar::~GameChar(void)
{
	GameObject::~GameObject();
	rigidBody->~RigidBody();
	meshRenderer->~MeshRenderer();
}

void GameChar::Update()
{
	GameObject::Update();
	rigidBody->Update();
}
