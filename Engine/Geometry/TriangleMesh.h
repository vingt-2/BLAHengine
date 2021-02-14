// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"
#include "StdInclude.h"

namespace BLA
{
    struct BLACORE_API RenderData
    {
        blaVector<blaU32> m_triangleIndices;
        blaVector<blaVec3>  m_vertPos;
        blaVector<blaVec3>  m_vertNormal;
        blaVector<blaVec3>  m_vertTangent;
        blaVector<blaVec3>  m_vertBiTangent;
        blaVector<blaVec2>  m_vertUVs;
    };

    class TriangleMesh
    {
    public:
        typedef blaU32 HeIndx;
        typedef blaU32 FaceIndx;

        BLACORE_API TriangleMesh() = default;
        BLACORE_API ~TriangleMesh() = default;

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

        blaVector<Face> m_meshTriangles;

        /*
        Now for Vertex specific data. Separate lists to simplify loading geometry data for rendering.
        */
        blaVector<blaVec3> m_vertexPos;
        blaVector<blaVec3> m_vertexNormals;
        blaVector<blaVec2> m_vertexUVs;

        /*
        Tangent information for each face:
        */
        blaVector<blaVec3> m_faceTangents;
        blaVector<blaVec3> m_faceBiTangents;

        /*
        Now we introduce Half-Edge specific blaMapping to fasten up adjacency queries
        */
        typedef struct HE_edge_t
        {
            DestVertex destVertex;
            FaceIndx borderingFace; // Triangle to the left of the half-edge 
            HeIndx nextHE; // next half-edge around that face.
            HeIndx oppositeHE; // opposite Half-Edge
        } HalfEdge;

        blaVector<HalfEdge> m_halfEdges; // The list of all half edges (should be 2*E)

        blaVector<HeIndx> m_heEmanatingFromVert; // a list of one HE index (from m_halfEdges) emanating from each vertex Position (should be size of m_vertexPos)

        blaVector<blaPair<blaString, blaSize>> m_materials;

        int m_manifoldViolationEdges = 0;

        RenderData m_renderData;

        BLACORE_API void BuildMeshTopo(
            blaVector<blaU32> vertPosIndices,
            blaVector<blaU32> vertNormalIndices,
            blaVector<blaU32> vertUVsIndices,
            bool swapNormals);

        BLACORE_API void NormalizeModelCoordinates(bool normalizeScale);
        BLACORE_API void ComputePerFaceTangentBasis();
        BLACORE_API void ApplyGeomScaling(blaVec3 scaling);
        BLACORE_API void ApplyUVScaling(blaVec2 scaling);

        BLACORE_API void GenerateRenderData();
        BLACORE_API void GenerateTopoTriangleIndices(blaVector<blaU32> &posIndices, blaVector<blaU32> &normalIndices);

        BLACORE_API bool IsMeshValid();

        BLACORE_API void ReverseEdgesOrder();
        BLACORE_API void GetHEvertices(HeIndx halfEdge, blaPair<blaU32, blaU32>* vertexPair);
        BLACORE_API void GetHETriangle(HeIndx halfEdge, FaceIndx* triangle);
        BLACORE_API bool GetSurroundingVertices(blaU32 vertexIndx, blaVector<DestVertex> &surroundingVertices);
        BLACORE_API bool GetSurroundingTriangles(blaU32 vertexIndx, blaVector<FaceIndx> &surroundingFaces);
        BLACORE_API bool GetEmanatingHalfEdges(blaU32 vertexIndx, blaVector<HeIndx> &edges);
        BLACORE_API void GetTriangleEdges(blaU32 triangle, HeIndx* edge0, HeIndx* edge1, HeIndx* edge2);
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
        long operator()(const blaPair<blaU32, blaU32> &key) const
        {
            return (key.first * 0x1f1f1f1f) ^ key.second;
        }
    };
}