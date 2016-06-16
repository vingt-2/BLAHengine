#include "MeshAsset.h"

PolygonalMesh::PolygonalMesh() {}
PolygonalMesh::~PolygonalMesh() {}

void PolygonalMesh::NormalizeModelCoordinates()
{
	float minX = this->m_vertexPos[0].x;
	float maxX = this->m_vertexPos[0].x;

	float minY = this->m_vertexPos[0].y;
	float maxY = this->m_vertexPos[0].y;

	float minZ = this->m_vertexPos[0].z;
	float maxZ = this->m_vertexPos[0].z;

	vec3 centroid = vec3(0, 0, 0);

	for (auto v : this->m_vertexPos)
	{
		minX = v.x < minX ? v.x : minX;
		maxX = v.x > maxX ? v.x : maxX;

		minY = v.y < minY ? v.y : minY;
		maxY = v.y > maxY ? v.y : maxY;

		minZ = v.z < minZ ? v.z : minZ;
		maxZ = v.z > maxZ ? v.z : maxZ;
		centroid += v;
	}

	centroid /= m_vertexPos.size();

	float correctX = maxX - minX;
	float correctY = maxY - minY;
	float correctZ = maxZ - minZ;

	for (int i = 0; i < m_vertexPos.size(); i++)
	{
		m_vertexPos[i] -= centroid;
	}
}

void PolygonalMesh::BuildHalfEdgeDS()
{

}
