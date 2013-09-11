#include "GameChar.h"


GameChar::GameChar(void)
{
	m_rigidBody		= new RigidBody(m_transform);
	m_meshRenderer	= new MeshRenderer(m_transform);
}


GameChar::~GameChar(void)
{
	GameObject::~GameObject();
	m_rigidBody->~RigidBody();
	m_meshRenderer->~MeshRenderer();
}

void GameChar::Update()
{
	this->UpdateTransform();
	m_rigidBody->Update();
}
