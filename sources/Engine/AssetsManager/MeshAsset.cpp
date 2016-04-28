#include "MeshAsset.h"

MeshAsset::MeshAsset() {}
MeshAsset::~MeshAsset() {}

void MeshAsset::NormalizeModelCoordinates()
{
	float minX = this->m_meshVertices[0].x;
	float maxX = this->m_meshVertices[0].x;

	float minY = this->m_meshVertices[0].y;
	float maxY = this->m_meshVertices[0].y;

	float minZ = this->m_meshVertices[0].z;
	float maxZ = this->m_meshVertices[0].z;

	vec3 centroid = vec3(0, 0, 0);

	for (auto v : this->m_meshVertices)
	{
		minX = v.x < minX ? v.x : minX;
		maxX = v.x > maxX ? v.x : maxX;

		minY = v.y < minY ? v.y : minY;
		maxY = v.y > maxY ? v.y : maxY;

		minZ = v.z < minZ ? v.z : minZ;
		maxZ = v.z > maxZ ? v.z : maxZ;
		centroid += v;
	}

	centroid /= m_meshVertices.size();

	float correctX = maxX - minX;
	float correctY = maxY - minY;
	float correctZ = maxZ - minZ;

	for (int i = 0; i < m_meshVertices.size(); i++)
	{
		m_meshVertices[i] -= centroid;

		//m_meshVertices[i].x /= correctX;

		//m_meshVertices[i].y /= correctY;

		//m_meshVertices[i].z /= correctZ;
	}
}