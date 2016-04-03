#pragma once
#include "MeshRenderer.h"
#include "../GameAlgebra/Ray.h"
#include "../GameObjects/GameChar.h"

#define OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>

using namespace ozcollide;

class Collider
{
public:
	Collider(GameChar* parent);
	~Collider();

	AABBTreePoly* m_baseTree;
	vector<Vec3f>* m_verticesInVec3f;
	vector<int>* m_triIndices;
	vector<Polygon>* m_triangles;

	bool CollidesWith(Collider* otherCollider);

private:

};