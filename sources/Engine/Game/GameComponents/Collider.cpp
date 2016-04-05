#include "Collider.h"

Collider::Collider(MeshRenderer* mesh)
{
	m_vertIndices = &(mesh->m_meshTriangles);
	m_triVertices = &(mesh->m_meshVertices);
	m_triNormals = &(mesh->m_meshNormals);

	m_collisionMesh = newCollisionModel3D();
	for (int i = 0; i < mesh->m_meshTriangles.size(); i +=3 )
	{
		uint32_t i0 = mesh->m_meshTriangles[i + 0];
		uint32_t i1 = mesh->m_meshTriangles[i + 1];
		uint32_t i2 = mesh->m_meshTriangles[i + 2];

		m_collisionMesh->addTriangle(&(mesh->m_meshVertices[i0].x),
			&(mesh->m_meshVertices[i1].x),
			&(mesh->m_meshVertices[i2].x));

	}
	m_collisionMesh->finalize();
}

Collider::~Collider()
{
	m_collisionMesh->~CollisionModel3D();
}