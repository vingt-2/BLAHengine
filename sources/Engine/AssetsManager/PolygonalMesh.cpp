#include "PolygonalMesh.h"

TriangleMesh::TriangleMesh() {}
TriangleMesh::~TriangleMesh() {}

void TriangleMesh::BuildMeshTopo(
	vector<uint32> vertPosIndices,
	vector<uint32> vertNormalIndices,
	vector<uint32> vertUVsIndices,
	bool swapNormals)
{
	unordered_map<pair<uint32_t, uint32_t>, uint32_t, uintPairHash> halfEdgesIndices; // an intermediate Data Structure to keep track of HEs
	vector<pair<uint32_t, uint32_t>> hePairs;

	m_heEmanatingFromVert.resize(vertPosIndices.size());

	for (uint32_t i = 0; i < vertPosIndices.size(); i += 3) // For each triangle
	{
		uint32_t currentTriangle = i / 3;

		uint32_t vindx0, vindx1, vindx2;
		uint32_t nindx0, nindx1, nindx2;
		uint32_t uvIndx0, uvIndx1, uvIndx2;

		if (swapNormals)
		{
			vindx0 = vertPosIndices[i + 2];
			vindx1 = vertPosIndices[i + 1];
			vindx2 = vertPosIndices[i + 0];
		}
		else
		{
			vindx0 = vertPosIndices[i + 1];
			vindx1 = vertPosIndices[i + 2];
			vindx2 = vertPosIndices[i + 0];
		}

		if (vertNormalIndices.empty())
		{
			nindx0 = -1;
			nindx1 = -1;
			nindx2 = -1;
		}
		else
		{
			nindx0 = vertNormalIndices[i + 1];
			nindx1 = vertNormalIndices[i + 2];
			nindx2 = vertNormalIndices[i + 0];
		}

		if (vertUVsIndices.empty())
		{
			uvIndx0 = -1;
			uvIndx1 = -1;
			uvIndx2 = -1;
		}
		else
		{
			uvIndx0 = vertUVsIndices[i + 1];
			uvIndx1 = vertUVsIndices[i + 2];
			uvIndx2 = vertUVsIndices[i + 0];
		}

		HalfEdge he0 = { vindx0, nindx0, uvIndx0, currentTriangle, i + 1, -1 }; // the -1 value is to be populated later
		HalfEdge he1 = { vindx1, nindx1, uvIndx1, currentTriangle, i + 2, -1 };
		HalfEdge he2 = { vindx2, nindx2, uvIndx2, currentTriangle, i + 0, -1 };

		m_halfEdges.push_back(he0);
		m_halfEdges.push_back(he1);
		m_halfEdges.push_back(he2);


		// Populating map and list to easily find opposite edges afterwards.
		uint32_t v0 = vertPosIndices.at(i);
		uint32_t v1 = vertPosIndices.at(i + 1);
		uint32_t v2 = vertPosIndices.at(i + 2);
		halfEdgesIndices[pair<uint32_t, uint32_t>(v0, v1)] = i;
		halfEdgesIndices[pair<uint32_t, uint32_t>(v1, v2)] = i + 1;
		halfEdgesIndices[pair<uint32_t, uint32_t>(v2, v0)] = i + 2;
		hePairs.push_back(pair<uint32_t, uint32_t>(v0, v1));
		hePairs.push_back(pair<uint32_t, uint32_t>(v1, v2));
		hePairs.push_back(pair<uint32_t, uint32_t>(v2, v0));

		// Populates m_vertEmanatingHE
		m_heEmanatingFromVert[v0] = i;
		m_heEmanatingFromVert[v1] = i + 1;
		m_heEmanatingFromVert[v2] = i + 2;

		// Populates m_triangleHE
		TriangleMesh::face face = { i };
		m_meshTriangles.push_back(face);
	}

	for (uint32_t i = 0; i < m_halfEdges.size(); i++)
	{
		TriangleMesh::HalfEdge* edge = &(m_halfEdges.at(i));

		pair<uint32_t, uint32_t> vertPair = hePairs[i];

		try
		{
			edge->oppositeHE = halfEdgesIndices.at(pair<uint32_t, uint32_t>(vertPair.second, vertPair.first));
		}
		catch (const out_of_range& e)
		{
			edge->oppositeHE = 0xFFFFFFFF;
			m_manifoldViolationEdges++;
		}
	}
}

void TriangleMesh::NormalizeModelCoordinates()
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

RenderData * TriangleMesh::GenerateRenderData()
{
	RenderData* renderData = new RenderData();

	unordered_map<RenderVertEntry, uint32_t, vertEntryHasher> vertexMap;

	for (uint32_t triIndx = 0; triIndx < m_meshTriangles.size(); triIndx ++)
	{
		HalfEdge edge0 = m_halfEdges[m_meshTriangles[triIndx].firstEdge];
		HalfEdge edge1 = m_halfEdges[edge0.nextHE];
		HalfEdge edge2 = m_halfEdges[edge1.nextHE];

		for (uint32_t i = 0; i < 3; i++)
		{
			HalfEdge edge;

			if (i == 0) edge = edge2;
			if (i == 1) edge = edge0;
			if (i == 2) edge = edge1;

			RenderVertEntry vert;

			vert.vx = m_vertexPos[edge.destVertex];
			vert.vn = m_vertexNormals[edge.destVertexNormal];
			vert.vt = m_vertexUVs[edge.destVertexUV];

			if (vertexMap.count(vert) == 0)
			{
				renderData->m_vertPos.push_back(vert.vx);
				renderData->m_vertNormal.push_back(vert.vn);
				renderData->m_vertUVs.push_back(vert.vt);

				vertexMap[vert] = renderData->m_vertPos.size() - 1;

				renderData->m_triangleIndices.push_back(renderData->m_vertPos.size() - 1);

			}
			else
			{
				renderData->m_triangleIndices.push_back(vertexMap[vert]);
			}
		}
	}

	return renderData;
}

vector<uint32_t> * TriangleMesh::GenerateTopoTriangleIndices()
{
	vector<uint32_t>* indices = new vector<uint32>();

	for (int triIndx = 0; triIndx < m_meshTriangles.size(); triIndx ++)
	{
		HalfEdge edge0 = m_halfEdges[m_meshTriangles[triIndx].firstEdge];
		HalfEdge edge1 = m_halfEdges[edge0.nextHE];
		HalfEdge edge2 = m_halfEdges[edge1.nextHE];

		for (int i = 0; i < 3; i++)
		{
			HalfEdge edge;

			if (i == 0) edge = edge2;
			if (i == 1) edge = edge0;
			if (i == 2) edge = edge1;

			indices->push_back(edge.destVertex);
		}
	}

	return indices;
}

bool TriangleMesh::IsMeshValid()
{
	bool check = true;
	if (m_vertexPos.empty())
	{
		check = false;
	}

	return check;
}

void TriangleMesh::ReverseEdgesOrder()
{
	// TODO: IMPLEMENT DAH
	//for (auto face : m_meshTriangles)
	//{
	//	//uint32_t temp;

	//	//HalfEdge edge0 = m_halfEdges[face.firstEdge];
	//	//edge0.
	//	//HalfEdge edge1 = m_halfEdges[edge0.nextHE];
	//	//HalfEdge edge2 = m_halfEdges[edge1.nextHE];
	//}
}

void TriangleMesh::GetHEvertices(heIndx halfEdge, pair<uint32, uint32>* vertexPair)
{
	HalfEdge edge = m_halfEdges[halfEdge];
	vertexPair->first = edge.destVertex;
	
	if (edge.oppositeHE != 0xFFFFFFFF)
	{
		vertexPair->second = m_halfEdges[edge.oppositeHE].destVertex;
	}
	else
	{
		// If no opposite HE exists, go around the existing face.
		heIndx findEdgeIndx = edge.nextHE;
		HalfEdge findEdge = m_halfEdges[findEdgeIndx];
		do
		{
			findEdgeIndx = findEdge.nextHE;
		} while (m_halfEdges[findEdgeIndx].nextHE != halfEdge);

		vertexPair->second = m_halfEdges[findEdgeIndx].destVertex;
	}
}

void TriangleMesh::GetHETriangle(heIndx halfEdge, faceIndx* triangle)
{
	*triangle = m_halfEdges[halfEdge].borderingFace;
}

void TriangleMesh::GetSurroundingVertices(uint32 vertexIndx, vector<uint32>* surroundingVertices)
{
	heIndx startEdgeIndx = m_heEmanatingFromVert[vertexIndx];
	heIndx currentEdgeIndx = startEdgeIndx;

	uint32 failCount = 0;

	HalfEdge edge;
	do
	{
		edge = m_halfEdges[startEdgeIndx];
		surroundingVertices->push_back(edge.destVertex);

		if (edge.oppositeHE != 0xFFFFFFFF)
		{
			currentEdgeIndx = m_halfEdges[edge.oppositeHE].nextHE;
		}
		else
		{
			failCount++;

			// Find inward edge along the same face (that we know exists), then take it's opposite so it's out of the vertex
			heIndx findEdgeIndx = edge.nextHE;
			HalfEdge findEdge = m_halfEdges[findEdgeIndx];
			do
			{
				findEdgeIndx = findEdge.nextHE;
			} while (m_halfEdges[findEdgeIndx].nextHE != currentEdgeIndx);

			currentEdgeIndx = m_halfEdges[findEdgeIndx].oppositeHE;
		}
	} while (currentEdgeIndx != startEdgeIndx && failCount < 2);
}

void TriangleMesh::GetTriangleEdges(int triangle, heIndx* edge0, heIndx* edge1, heIndx* edge2)
{
	*edge0 = m_meshTriangles[triangle].firstEdge;
	*edge1 = m_halfEdges[*edge0].nextHE;
	*edge2 = m_halfEdges[*edge1].nextHE;
}
