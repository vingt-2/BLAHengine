#pragma once
#include "../../Common/Maths.h"
#include "../../Common/std.h"

class RenderData
{
public:

	vector<uint32_t> m_triangleIndices;
	vector<vec3> m_vertPos;
	vector<vec3> m_vertNormal;
	vector<vec3> m_vertTangent;
	vector<vec3> m_vertBiTangent;
	vector<vec2> m_vertUVs;

	RenderData() {};
	~RenderData() 
	{
		m_triangleIndices.clear();
		m_vertPos.clear();
		m_vertNormal.clear();
		m_vertTangent.clear();
		m_vertBiTangent.clear();
		m_vertUVs.clear();
	}
};

class TriangleMesh
{
public:
	typedef uint32 heIndx;
	typedef uint32 faceIndx;

	typedef struct face_t
	{
		heIndx firstEdge;
	} face;

	vector<face> m_meshTriangles;
	
	/*
		Now for Vertex specific data. Separate lists to simplify loading geometry data for rendering.
	*/
	vector<vec3> m_vertexPos;
	vector<vec3> m_vertexNormals;
	vector<vec2> m_vertexUVs;

	/*
		Now we introduce Half-Edge specific mapping to fasten up adjacency queries
	*/
	typedef struct HE_edge_t 
	{
		uint32_t destVertex; // Vertex the HE points to.
		uint32_t destVertexNormal; // Normals at the destination Vertex (may be unique to this face)
		uint32_t destVertexUV; // UV at the destination Vertex (may be unique to this face)
		faceIndx borderingFace; // Triangle to the left of the half-edge 
		heIndx nextHE; // next half-edge around that face.
		heIndx oppositeHE; // opposite Half-Edge
	} HalfEdge;

	vector<HalfEdge> m_halfEdges; // The list of all half edges (should be 2*E)

	vector<heIndx> m_heEmanatingFromVert; // a list of one HE index (from m_halfEdges) emanating from each vertex Position (should be size of m_vertexPos)

	int m_manifoldViolationEdges = 0;

	TriangleMesh();
	~TriangleMesh();

	void NormalizeModelCoordinates();
	RenderData* GenerateRenderData();
	vector<uint32_t>* GenerateTopoTriangleIndices();

	bool IsMeshValid();

	void ReverseEdgesOrder();
	void GetHEvertices(heIndx halfEdge, pair<uint32, uint32>* vertexPair);
	void GetHETriangle(heIndx halfEdge, faceIndx* triangle);
	void GetSurroundingVertices(uint32 vertexIndx, vector<uint32>* surroundingVertices);
	void GetTriangleEdges(int triangle, heIndx* edge0, heIndx* edge1, heIndx* edge2);
};

typedef struct RenderVertEntry_t {
	vec3 vx;
	vec3 vn;
	vec2 vt;

	bool operator==(const RenderVertEntry_t& other) const
	{
		float epsilon = 0.0001f;

		if (glm::distance(vx, other.vx) > epsilon)
		{
			return false;
		}

		if (glm::distance(vn, other.vn) > epsilon)
		{
			return false;
		}

		if (glm::distance(vt, other.vt) > epsilon)
		{
			return false;
		}
	}
} RenderVertEntry;

typedef struct TopoVertEntry_t {
	vec3 vx;
	vec3 vn;
	vec2 vt;

	// We test only for equality of position as we use purely spatial hashing to
	// keep the integrity of the Polygonal mesh topology
	// (Variations in vertex attribute should not reflect in fragmented topology and non-manifoldness)
	// Fragmentation happens when generating render data.
	bool operator==(const TopoVertEntry_t& other) const
	{
		float epsilon = 0.0001;

		if (glm::distance(vx, other.vx) > epsilon)
		{
			return false;
		}
		return true;
	}
} TopoVertEntry;

struct vertEntryHasher
{
	std::size_t operator()(const TopoVertEntry& v) const
	{
		float hashResolution = 0.0001f;

		long celPosX = (long)(v.vx.x / hashResolution);
		long celPosY = (long)(v.vx.y / hashResolution);
		long celPosZ = (long)(v.vx.z / hashResolution);

		int p1 = 73856093;
		int p2 = 19349663;
		int p3 = 83492791;

		int hashVx = (int)(100 * v.vx.x)*p1 ^ (int)(100 * v.vx.y)*p2 ^ (int)(100 * v.vx.z)*p3;

		return hashVx;
	}

	std::size_t operator()(const RenderVertEntry& v) const
	{
		float hashResolution = 0.0001f;

		long celPosX = (long)(v.vx.x / hashResolution);
		long celPosY = (long)(v.vx.y / hashResolution);
		long celPosZ = (long)(v.vx.z / hashResolution);

		int p1 = 73856093;
		int p2 = 19349663;
		int p3 = 83492791;

		int hashVx = (int)(100 * v.vx.x)*p1 ^ (int)(100 * v.vx.y)*p2 ^ (int)(100 * v.vx.z)*p3;

		return hashVx;
	}
};


struct uintPairHash
{
	long operator()(const pair<uint32_t, uint32_t> &key) const
	{
		return (key.first * 0x1f1f1f1f) ^ key.second;
	}
};