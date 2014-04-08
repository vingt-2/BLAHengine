#pragma once
#include "MeshRenderer.h"
#include "../GameAlgebra/Ray.h"

class BoxCollider
{
public:
	BoxCollider(const vec3 &min, const vec3 &max)
	{
		assert(min.x < max.x);
		assert(min.y < max.y);
		assert(min.z < max.z);

		m_bounds[0] = min;
		m_bounds[1] = max;
	}

	bool RayIntersection(const Ray &ray, float startInterval, float endInterval);
	vec3 m_bounds[2];
};

class MeshCollider
{
public:
	MeshCollider(const vector<vec3> &vertices)
	{
	
	}

	bool RayIntersect(const Ray &ray, float startInterval, float endInterval);
	vector<vec3> m_vertices;
};

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