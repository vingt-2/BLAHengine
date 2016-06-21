#include "PolygonalMesh.h"

TriangleMesh::TriangleMesh() {}
TriangleMesh::~TriangleMesh() {}

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
