#pragma once
#include "Asset.h"
#include "../../Common/Maths/Maths.h"
#include "../../Common/StdInclude.h"

namespace BLAengine
{
    class BLACORE_API RenderData
    {
    public:

        vector<glm::uint32> m_triangleIndices;
        vector<blaVec3> m_vertPos;
        vector<blaVec3> m_vertNormal;
        vector<blaVec3> m_vertTangent;
        vector<blaVec3> m_vertBiTangent;
        vector<glm::vec2> m_vertUVs;

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

    class BLACORE_API TriangleMesh : public Asset
    {
    public:
        typedef glm::uint32 HeIndx;
        typedef glm::uint32 FaceIndx;

        typedef struct face_t
        {
            HeIndx firstEdge;
        } Face;

        typedef struct destVert_t
        {
            glm::uint32 pos; // Vertex the HE points to.
            glm::uint32 normal; // Normals at the destination Vertex (may be unique to this face)
            glm::uint32 UV; // UV at the destination Vertex (may be unique to this face)
        } DestVertex;

        vector<Face> m_meshTriangles;

        /*
        Now for Vertex specific data. Separate lists to simplify loading geometry data for rendering.
        */
        vector<blaVec3> m_vertexPos;
        vector<blaVec3> m_vertexNormals;
        vector<glm::vec2> m_vertexUVs;

        /*
        Tangent information for each face:
        */
        vector<blaVec3> m_faceTangent;

        /*
        Now we introduce Half-Edge specific mapping to fasten up adjacency queries
        */
        typedef struct HE_edge_t
        {
            DestVertex destVertex;
            FaceIndx borderingFace; // Triangle to the left of the half-edge 
            HeIndx nextHE; // next half-edge around that face.
            HeIndx oppositeHE; // opposite Half-Edge
        } HalfEdge;

        vector<HalfEdge> m_halfEdges; // The list of all half edges (should be 2*E)

        vector<HeIndx> m_heEmanatingFromVert; // a list of one HE index (from m_halfEdges) emanating from each vertex Position (should be size of m_vertexPos)

        int m_manifoldViolationEdges = 0;

        RenderData m_renderData;

        TriangleMesh(string name);
        ~TriangleMesh();

        void BuildMeshTopo(
            vector<glm::uint32> vertPosIndices,
            vector<glm::uint32> vertNormalIndices,
            vector<glm::uint32> vertUVsIndices,
            bool swapNormals);

        void NormalizeModelCoordinates(bool normalizeScale);
        void ComputeFaceTangents();
        void ApplyGeomScaling(blaVec3 scaling);
        void ApplyUVScaling(glm::vec2 scaling);

        void GenerateRenderData();
        void GenerateTopoTriangleIndices(vector<glm::uint32> &posIndices, vector<glm::uint32> &normalIndices);

        bool IsMeshValid();

        void ReverseEdgesOrder();
        void GetHEvertices(HeIndx halfEdge, pair<glm::uint32, glm::uint32>* vertexPair);
        void GetHETriangle(HeIndx halfEdge, FaceIndx* triangle);
        bool GetSurroundingVertices(glm::uint32 vertexIndx, vector<DestVertex> &surroundingVertices);
        bool GetSurroundingTriangles(glm::uint32 vertexIndx, vector<FaceIndx> &surroundingFaces);
        bool GetEmanatingHalfEdges(glm::uint32 vertexIndx, vector<HeIndx> &edges);
        void GetTriangleEdges(glm::uint32 triangle, HeIndx* edge0, HeIndx* edge1, HeIndx* edge2);
    };

    typedef struct RenderVertEntry_t
    {
        blaVec3 vx;
        blaVec3 vn;
        glm::vec2 vt;

        bool operator==(const RenderVertEntry_t& other) const
        {
            float epsilon = 0.001f;

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

            return true;
        }
    } RenderVertEntry;

    typedef struct TopoVertEntry_t
    {
        blaVec3 vx;
        blaVec3 vn;
        glm::vec2 vt;

        // We test only for equality of position as we use purely spatial hashing to
        // keep the integrity of the Polygonal mesh topology
        // (Variations in vertex attribute should not reflect in fragmented topology and non-manifoldness)
        // Fragmentation happens when generating render data.
        bool operator==(const TopoVertEntry_t& other) const
        {
            float epsilon = 0.001f;

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
            float hashResolution = 0.001f;

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
            float hashResolution = 0.001f;

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
        long operator()(const pair<glm::uint32, glm::uint32> &key) const
        {
            return (key.first * 0x1f1f1f1f) ^ key.second;
        }
    };
}