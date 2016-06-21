#include "GameChar.h"


GameChar::GameChar()
{
	m_meshRenderer = new MeshRenderer(m_transform);

	m_rigidBody = new RigidBody(m_transform);
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

void GameChar::SetTriangleMesh(TriangleMesh * mesh)
{
	m_meshRenderer->AssignTriangleMesh(mesh);

}
