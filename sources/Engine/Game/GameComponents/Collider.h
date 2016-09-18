#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "..\..\Assets\PolygonalMesh.h"
#include "../GameAlgebra/Ray.h"

// Collision detection Library !
#include "../../Ext/libcoldet/coldet.h"


class BLACORE_API Collider
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