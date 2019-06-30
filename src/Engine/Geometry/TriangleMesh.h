#pragma once

#include <Common/Maths/Maths.h>
#include <Common/StdInclude.h>

namespace BLAengine
{
    struct BLACORE_API RenderData
    {
        vector<blaU32>  m_triangleIndices;
        vector<blaVec3> m_vertPos;
        vector<blaVec3> m_vertNormal;
        vector<blaVec3> m_vertTangent;
        vector<blaVec3> m_vertBiTangent;
        vector<blaVec2> m_vertUVs;
    };

    class BLACORE_API TriangleMesh
    {
    public:
        typedef blaU32 HeIndx;
        typedef blaU32 FaceIndx;

        TriangleMesh() = default;
        ~TriangleMesh() = default;

        typedef struct face_t
        {
            HeIndx firstEdge;
        } Face;

        typedef struct destVert_t
        {
            blaU32 posIndex; // Vertex the HE points to.
            blaU32 normalIndex; // Normals at the destination Vertex (may be unique to this face)
            blaU32 uvIndex; // UV at the destination Vertex (may be unique to this face)
        } DestVertex;

        vector<Face> m_meshTriangles;

        /*
        Now for Vertex specific data. Separate lists to simplify loading geometry data for rendering.
        */
        vector<blaVec3> m_vertexPos;
        vector<blaVec3> m_vertexNormals;
        vector<blaVec2> m_vertexUVs;

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

        void BuildMeshTopo(
            vector<blaU32> vertPosIndices,
            vector<blaU32> vertNormalIndices,
            vector<blaU32> vertUVsIndices,
            bool swapNormals);

        void NormalizeModelCoordinates(bool normalizeScale);
        void ComputeFaceTangents();
        void ApplyGeomScaling(blaVec3 scaling);
        void ApplyUVScaling(glm::vec2 scaling);

        void GenerateRenderData();
        void GenerateTopoTriangleIndices(vector<blaU32> &posIndices, vector<blaU32> &normalIndices);

        bool IsMeshValid();

        void ReverseEdgesOrder();
        void GetHEvertices(HeIndx halfEdge, pair<blaU32, blaU32>* vertexPair);
        void GetHETriangle(HeIndx halfEdge, FaceIndx* triangle);
        bool GetSurroundingVertices(blaU32 vertexIndx, vector<DestVertex> &surroundingVertices);
        bool GetSurroundingTriangles(blaU32 vertexIndx, vector<FaceIndx> &surroundingFaces);
        bool GetEmanatingHalfEdges(blaU32 vertexIndx, vector<HeIndx> &edges);
        void GetTriangleEdges(blaU32 triangle, HeIndx* edge0, HeIndx* edge1, HeIndx* edge2);
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
        long operator()(const pair<blaU32, blaU32> &key) const
        {
            return (key.first * 0x1f1f1f1f) ^ key.second;
        }
    };
}