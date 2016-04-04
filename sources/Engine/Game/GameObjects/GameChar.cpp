#include "GameChar.h"


GameChar::GameChar(MeshAsset* mesh)
{
	m_meshRenderer = new MeshRenderer(m_transform);

	if (mesh != NULL)
	{
		m_meshRenderer->m_meshTriangles = mesh->m_meshTriangles;
		m_meshRenderer->m_meshVertices = mesh->m_meshVertices;
		m_meshRenderer->m_meshNormals = mesh->m_meshNormals;
		m_meshRenderer->m_meshUVs = mesh->m_meshUVs;
	}

	m_rigidBody = new RigidBody(m_transform,m_meshRenderer);
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
