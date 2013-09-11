#pragma once
#include "MeshRenderer.h"

class Collider
{

public:

	Collider(Transform* parentTransform);
	~Collider();

	vector<vec3> m_meshVertices;
    
	MeshRenderer* m_colliderRenderer;
    
    GLfloat m_boundingSphereRadius;

	Transform* m_parentTransform;

	void GenerateBoundingBoxFromMesh(MeshRenderer* meshRenderer);
	void GenerateBoundingSphereRadius(MeshRenderer* meshRenderer);
    void UpdateRenderer();

    
};