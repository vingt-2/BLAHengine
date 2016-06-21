#pragma once
#include "MeshRenderer.h"
#include "../GameAlgebra/Ray.h"

#include "../../Ext/libcoldet/coldet.h"


class Collider
{
public:
	Collider(TriangleMesh* mesh);
	~Collider();
	
	CollisionModel3D* m_collisionMesh;
	vector<uint32_t>* m_vertIndices;
	vector<vec3>* m_triVertices;
	vector<vec3>* m_triNormals;

	float m_boundingRadius;

private:
	void GenerateBoundingRadius();
	void GenerateCollisionModel();
};