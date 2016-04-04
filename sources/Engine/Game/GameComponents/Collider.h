#pragma once
#include "MeshRenderer.h"
#include "../GameAlgebra/Ray.h"

#include "../../Ext/libcoldet/coldet.h"


class Collider
{
public:
	Collider(MeshRenderer* mesh);
	~Collider();
	
	CollisionModel3D* m_collisionMesh;
	vector<vec3>* m_triVertices;
	vector<uint32_t>* m_faces;
};