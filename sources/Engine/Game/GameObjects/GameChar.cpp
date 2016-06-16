#include "GameChar.h"


GameChar::GameChar(PolygonalMesh* mesh)
{
	m_meshRenderer = new MeshRenderer(m_transform, mesh);

	m_meshRenderer->m_mesh = mesh;

	m_rigidBody = new RigidBody(m_transform, mesh);
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
