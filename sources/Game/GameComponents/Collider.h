#pragma once
#include "MeshRenderer.h"

class Collider
{

public:

	Collider(Transform* parentTransform);
	~Collider();

	vector<vec3> m_meshVertices;
	MeshRenderer* m_colliderRenderer;

	Transform* m_parentTransform;

	void GenerateBoundingBoxFromMesh(MeshRenderer* meshRenderer);
	void UpdateRenderer();

};