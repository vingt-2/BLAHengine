#include "Collider.h"

Collider::Collider(Transform* parentTransform):
	m_meshVertices(vector<vec3>()),
	m_colliderRenderer(new MeshRenderer(parentTransform))
{
	m_parentTransform = parentTransform;
	m_colliderRenderer->m_renderType = BLA_LINE_RENDER;
}

Collider::~Collider()
{
}

void Collider::GenerateBoundingBoxFromMesh(MeshRenderer* meshRenderer)
{
	cout << "[Collider] Generating Bounding Box From Mesh: ";

	GLfloat maxX, maxY, maxZ, minX, minY, minZ;

	if (meshRenderer->m_meshVertices.size() > 0)
	{

		for (GLuint i = 0; i < meshRenderer->m_meshVertices.size(); i++)
		{
			vec3 vertex = meshRenderer->m_meshVertices.at(i);
			if (i == 0)
			{
				maxX = vertex.x;
				minX = vertex.x;
				maxY = vertex.y;
				minY = vertex.y;
				maxZ = vertex.z;
				minZ = vertex.z;
			}
			else
			{
				if (vertex.x > maxX)
				{
					maxX = vertex.x;
				}
				if (vertex.x < minX)
				{
					minX = vertex.x;
				}
				if (vertex.y > maxY)
				{
					maxY = vertex.y;
				}
				if (vertex.y < minY)
				{
					minY = vertex.y;
				}
				if (vertex.z > maxZ)
				{
					maxZ = vertex.z;
				}
				if (vertex.z < minZ)
				{
					minZ = vertex.z;
				}
			}
		}
	}
	else
	{
		// Generate a canonical bounding box.
		maxX = 1.f;
		minX = -1.f;
		maxY = 1.f;
		minY = -1.f;
		maxZ = 1.f;
		minZ = -1.f;
	}

	cout << "(" << minX << "," << minY << "," << minZ << ")" <<  "(" << maxX << "," << maxY << "," << maxZ << "). \n";
	m_meshVertices.push_back(vec3(maxX,maxY,maxZ));
	m_meshVertices.push_back(vec3(maxX,maxY,minZ));
	m_meshVertices.push_back(vec3(maxX,minY,maxZ));
	m_meshVertices.push_back(vec3(maxX,minY,minZ));
	m_meshVertices.push_back(vec3(minX,maxY,maxZ));
	m_meshVertices.push_back(vec3(minX,maxY,minZ));
	m_meshVertices.push_back(vec3(minX,minY,maxZ));
	m_meshVertices.push_back(vec3(minX,minY,minZ));
}
void Collider::GenerateBoundingSphereRadius(MeshRenderer *meshRenderer)
{
	/*float maxVertexLength = meshRenderer->m_meshVertices.at(0).length();
	for(GLuint i = 1; i<meshRenderer->m_meshVertices.size(); i++)
	{
		vec3 vertex = meshRenderer->m_meshVertices.at(i);
		
		if(maxVertexLength < vertex.length())
		{
			maxVertexLength = vertex.length();
		}
	}
	m_boundingSphereRadius = maxVertexLength;*/
}

bool BoxCollider::RayIntersection(const Ray &ray, float startInterval, float endInterval)
{
	/*
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
		tmax = (bounds[1 - r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
		tymin = (bounds[r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
		tymax = (bounds[1 - r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
		if ((tmin > tymax) || (tymin > tmax))
			return false;
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;
		tzmin = (bounds[r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
		tzmax = (bounds[1 - r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
		if ((tmin > tzmax) || (tzmin > tmax))
			return false;
		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			3
			tmax = tzmax;
	*/
	return false;
}