#include "GameChar.h"


GameChar::GameChar(void)
{
	rigidBody		= new RigidBody(transform);
	meshRenderer	= new MeshRenderer(transform);
}


GameChar::~GameChar(void)
{
}

void GameChar::Update()
{
	GameObject::Update();
	rigidBody->Update();
}
