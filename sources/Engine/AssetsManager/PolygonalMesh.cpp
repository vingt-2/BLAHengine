#include "PolygonalMesh.h"

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

	for (uint32_t i = 0; i < m_vertexPos.size(); i++)
	{
		m_vertexPos[i] -= centroid;
	}
}

struct uintPairHash
{
	long operator()(const pair<uint32_t, uint32_t> &key) const
	{
		return (key.first * 0x1f1f1f1f) ^ key.second;
	}
};

void PolygonalMesh::BuildHalfEdgeDS()
{
	/*
		Alright, Here we scroll through every face collected in the Mesh import.
		For each face, we extract all half-edges bordering the face.
		We keep a map of pairs <vertexIndx, vertexIndx> to accelerate the query
		of existing Half-edges.

	*/
	
	for (uint32_t i = 0; i < m_vertexPos.size(); i++)
	{
		m_vertEmanatingHE.push_back(-1); // -1 value to be populated later
	}

	unordered_map<pair<uint32_t, uint32_t>, uint32_t, uintPairHash> halfEdgesIndices; // an intermediate Data Structure to keep track of HEs
	vector<pair<uint32_t, uint32_t>> hePairs;

	for (uint32_t i = 0; i < m_meshTriangles.size(); i += 3)
	{
		uint32_t currentTriangle = i / 3;

		uint32_t v0 = m_meshTriangles.at(i);
		uint32_t v1 = m_meshTriangles.at(i + 1);
		uint32_t v2 = m_meshTriangles.at(i + 2);

		HE_edge he0 = { currentTriangle, v1, i + 1, -1 }; // the -1 value is to be populated later
		HE_edge he1 = { currentTriangle, v2, i + 2, -1 };
		HE_edge he2 = { currentTriangle, v0, i, -1 };

		m_halfEdges.push_back(he0);
		m_halfEdges.push_back(he1);
		m_halfEdges.push_back(he2);

		// Populating map and list to easily find opposite edges afterwards.
		halfEdgesIndices[pair<uint32_t, uint32_t>(v0, v1)] = i;
		halfEdgesIndices[pair<uint32_t, uint32_t>(v1, v2)] = i + 1;
		halfEdgesIndices[pair<uint32_t, uint32_t>(v2, v0)] = i + 2;
		hePairs.push_back(pair<uint32_t, uint32_t>(v0, v1));
		hePairs.push_back(pair<uint32_t, uint32_t>(v1, v2));
		hePairs.push_back(pair<uint32_t, uint32_t>(v2, v0));

		// Populates m_vertEmanatingHE
		m_vertEmanatingHE[v0] = i;
		m_vertEmanatingHE[v1] = i + 1;
		m_vertEmanatingHE[v2] = i + 2;

		// Populates m_triangleHE
		m_triangleHE.push_back(i);
	}

	int problematicEdges = 0;

	for (uint32_t i = 0; i < m_halfEdges.size(); i++)
	{
		HE_edge* edge = &(m_halfEdges.at(i));

		pair<uint32_t, uint32_t> vertPair = hePairs[i];


		try
		{
			edge->oppositeHE = halfEdgesIndices.at(pair<uint32_t, uint32_t>(vertPair.second, vertPair.first));
		}
		catch (const out_of_range& e)
		{
			edge->oppositeHE = 0xFFFFFFFF;
			problematicEdges++;
		}
	}
	m_manifoldViolationEdges = problematicEdges;
}

bool PolygonalMesh::IsMeshValid()
{
	bool check = true;
	if (m_vertexPos.empty())
	{
		check = false;
	}

	return check;
}
