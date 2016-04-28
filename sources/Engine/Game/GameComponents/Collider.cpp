#include "Collider.h"

Collider::Collider(MeshRenderer* mesh)
{
	m_vertIndices = &(mesh->m_meshTriangles);
	m_triVertices = &(mesh->m_meshVertices);
	m_triNormals = &(mesh->m_meshNormals);

	GenerateBoundingRadius();
	GenerateCollisionModel();
}

Collider::~Collider()
{
	m_collisionMesh->~CollisionModel3D();
}

void Collider::GenerateBoundingRadius()
{
	vec3 maxVert = vec3(0);

	for (auto v : *m_triVertices)
	{
		if (length(v) > length(maxVert))
		{
			maxVert = v;
		}
	}

	m_boundingRadius = length(maxVert);
}

void Collider::GenerateCollisionModel()
{
	m_collisionMesh = newCollisionModel3D();
	for (int i = 0; i < m_vertIndices->size(); i += 3)
	{
		uint32_t i0 = m_vertIndices->at(i + 0);
		uint32_t i1 = m_vertIndices->at(i + 1);
		uint32_t i2 = m_vertIndices->at(i + 2);

		m_collisionMesh->addTriangle(&(m_triVertices->at(i0).x),
			&(m_triVertices->at(i1).x),
			&(m_triVertices->at(i2).x));

	}
	m_collisionMesh->finalize();
}
