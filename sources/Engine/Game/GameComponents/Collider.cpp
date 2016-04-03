#include "Collider.h"
Collider::Collider(GameChar* parentObject)
{
	MeshRenderer* mesh = parentObject->m_meshRenderer;
	
	m_verticesInVec3f = new vector<ozcollide::Vec3f>();
	for (auto v : mesh->m_meshVertices)
	{
		m_verticesInVec3f->push_back(ozcollide::Vec3f(v.x, v.y, v.z));
	}

	m_triIndices = new vector<int>();
	for (int tri = 0; tri < mesh->m_meshTriangles.size(); tri++)
	{
		m_triIndices->push_back(mesh->m_meshTriangles[tri]);
	}
	
	m_triangles = new vector<Polygon>();
	for (int tri = 0; tri < mesh->m_meshTriangles.size(); tri += 3)
	{
		int i = mesh->m_meshTriangles[tri];

		Polygon p;//we must build each face before add it to the list
		ozcollide::Vec3f tmpNormal(mesh->m_meshNormals[i].x, mesh->m_meshNormals[i].y, mesh->m_meshNormals[i].z);

		p.setIndicesMemory(3, &(m_triIndices->at(tri)));//setting up indices

		p.setNormal(tmpNormal);//adding normals (previously readed)
		m_triangles->push_back(p);//adding the polygon to the polygon list
	}

	AABBTreePolyBuilder builder;
	m_baseTree = builder.buildFromPolys(m_triangles->data(),//polygons
		m_triangles->size(),//polygon count
		m_verticesInVec3f->data(),//vertices
		m_verticesInVec3f->size());//vertices count
}

Collider::~Collider()
{
	m_baseTree->destroy();
	m_verticesInVec3f->clear();
	m_triIndices->clear();
	m_triangles->clear();
}

bool Collider::CollidesWith(Collider* otherCollider)
{

	return true;
}