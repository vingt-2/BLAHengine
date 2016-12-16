#include "Collider.h"
using namespace BLAengine;

inline vec3 Barycentric(vec3 p, vec3 a, vec3 b, vec3 c)
{
	vec3 r;
	vec3 v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = dot(v0, v0);
	float d01 = dot(v0, v1);
	float d11 = dot(v1, v1);
	float d20 = dot(v2, v0);
	float d21 = dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	r.y = (d11 * d20 - d01 * d21) / denom;
	r.z = (d00 * d21 - d01 * d20) / denom;
	r.x = 1.0f - r.y - r.z;
	return r;
}

MeshCollider::MeshCollider(TriangleMesh* mesh)
{
	m_vertPosIndices = new vector<uint32_t>;
	m_vertNormalIndices = new vector<uint32_t>;
	mesh->GenerateTopoTriangleIndices(*m_vertPosIndices, *m_vertNormalIndices);
	m_triVertices = &(mesh->m_vertexPos);
	m_triNormals = &(mesh->m_vertexNormals);

	GenerateBoundingRadius();
	GenerateCollisionModel();
}

MeshCollider::~MeshCollider()
{
	m_collisionMesh->~CollisionModel3D();
	m_vertPosIndices->clear();
	m_vertNormalIndices->clear();
	m_vertPosIndices->~vector();
	m_vertNormalIndices->~vector();
}

Collider::RayCollision MeshCollider::CollideWithRay(Ray & ray)
{
	Transform transform = this->GetObjectTransform();
	vec3 hitInWorld;

	this->m_collisionMesh->setTransform(&(transform.m_transformMatrix[0][0]));

	int triangleIndex;
	float colT;
	vec3 colPointL;

	bool collision = this->m_collisionMesh->threadSafeClosestRayCollision(&(ray.m_origin.x), &(ray.m_direction.x), triangleIndex, colT, &(colPointL.x));

	MeshCollider::RayCollision contactPoint;
	if (!collision)
	{
		contactPoint.m_isValid = false;
	}
	else
	{
		contactPoint.m_isValid = true;
		contactPoint.m_colPositionL = colPointL;
		contactPoint.m_colPositionW = transform.LocalPositionToWorld(colPointL);

		vec3 contactNormals[3] = { vec3(0), vec3(0), vec3(0) };
		vec3 contactVertices[3] = { vec3(0), vec3(0), vec3(0) };

		for (int k = 0; k < 3; k++)
		{
			uint32_t vertPosIndex = this->m_vertPosIndices->at(3 * triangleIndex + k);
			contactVertices[k] = this->m_triVertices->at((int)vertPosIndex);
			uint32_t vertNormalIndex = this->m_vertNormalIndices->at(3 * triangleIndex + k);
			if (m_triNormals->size() != 0) contactNormals[k] = m_triNormals->at((int)vertNormalIndex);
		}
		vec3 bar = Barycentric(contactPoint.m_colPositionL, contactVertices[0], contactVertices[1], contactVertices[2]);
		//contactPoint.m_colNormalL = 0.3333333333f * (contactNormals[0] + contactNormals[1] + contactNormals[2]);
		contactPoint.m_colNormalL = bar.x * contactNormals[0] + bar.y * contactNormals[1] + bar.z * contactNormals[2];
		contactPoint.m_colNormalW = transform.LocalDirectionToWorld(contactPoint.m_colNormalL);
		contactPoint.m_t = colT;
	}
	return contactPoint;
}

Collider::RayCollision MeshCollider::CollideWithCollider(Collider & collider)
{
	return RayCollision();
}

void MeshCollider::GenerateBoundingRadius()
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

void MeshCollider::GenerateCollisionModel()
{
	m_collisionMesh = newCollisionModel3D();
	for (int i = 0; i < m_vertPosIndices->size(); i += 3)
	{
		uint32_t i0 = m_vertPosIndices->at(i + 0);
		uint32_t i1 = m_vertPosIndices->at(i + 1);
		uint32_t i2 = m_vertPosIndices->at(i + 2);

		m_collisionMesh->addTriangle(&(m_triVertices->at(i0).x),
			&(m_triVertices->at(i1).x),
			&(m_triVertices->at(i2).x));

	}
	m_collisionMesh->finalize();
}

Collider::RayCollision SphereCollider::CollideWithRay(Ray& ray)
{
	Collider::RayCollision contactPoint;
	Transform transform = this->GetObjectTransform();

	vec3 op = transform.m_position - ray.m_origin;
	double t, eps = 1e-4;
	double b = dot(op, ray.m_direction);
	double det = b * b - dot(op,op) + this->m_boundingRadius * this->m_boundingRadius;

	if (det < 0)
	{
		contactPoint.m_isValid = false;
	}
	else
	{
		det = sqrt(det);
		t = (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);

		if (t == 0)
		{
			contactPoint.m_isValid = false;
		}
		else
		{
			contactPoint.m_isValid = true;
			contactPoint.m_colPositionW = ray.m_origin + ((float) t) * ray.m_direction;
			contactPoint.m_colPositionL = transform.WorldPositionToLocal(contactPoint.m_colPositionW);
			
			//printVector(contactPoint.m_colPositionW);

			contactPoint.m_colNormalW = (contactPoint.m_colPositionW - transform.m_position) / m_boundingRadius;

			contactPoint.m_colNormalL = (contactPoint.m_colPositionL - transform.m_position);

			contactPoint.m_t = t;
		}
	}
	return contactPoint;
}

Collider::RayCollision SphereCollider::CollideWithCollider(Collider & collider)
{
	return RayCollision();
}
