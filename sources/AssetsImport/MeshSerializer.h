#pragma once

#include "../Common/Maths.h"
#include "../Engine/Assets/PolygonalMesh.h"
#include "MathSerializer.h"
#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>


class TriangleMeshSerializer
{
public:
    class FaceSerializer
    {
    public:
        FaceSerializer() = default;

        void FillData(BLAengine::TriangleMesh::Face f)
        {
            firstEdge = f.firstEdge;
        }

        void LoadData(BLAengine::TriangleMesh::Face &f)
        {
            f.firstEdge = firstEdge;
        }

    private:
        BLAengine::TriangleMesh::HeIndx firstEdge;

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive & archive)
        {
            archive
            (
                firstEdge
            );
        }
    };

    class DestVertexSerializer
    {
    public:
        DestVertexSerializer() = default;

        void FillData(BLAengine::TriangleMesh::DestVertex dvert)
        {
            pos = dvert.pos;
            normal = dvert.normal;
            UV = dvert.UV;
        }

        void LoadData(BLAengine::TriangleMesh::DestVertex &dvert)
        {
            dvert.pos = pos;
            dvert.normal = normal;
            dvert.UV = UV;
        }

        uint32_t pos; // Vertex the HE points to.
        uint32_t normal; // Normals at the destination Vertex (may be unique to this face)
        uint32_t UV; // UV at the destination Vertex (may be unique to this face)

    private:
        friend class cereal::access;

        template <class Archive>
        void serialize(Archive & archive)
        {
            archive
            (
                pos, normal, UV
            );
        }
    };

    class HalfEdgeSerializer
    {
    public:
        HalfEdgeSerializer() = default;

        void FillData(BLAengine::TriangleMesh::HalfEdge he)
        {
            destVertex = DestVertexSerializer();
            destVertex.FillData(he.destVertex);

            borderingFace = he.borderingFace;
            nextHE = he.nextHE;
            oppositeHE = he.oppositeHE;
        }

        void LoadData(BLAengine::TriangleMesh::HalfEdge &he)
        {
            destVertex.LoadData(he.destVertex);
            he.borderingFace = borderingFace;
            he.nextHE = nextHE;
            he.oppositeHE = oppositeHE;
        }

        DestVertexSerializer destVertex;
        BLAengine::TriangleMesh::FaceIndx borderingFace;
        BLAengine::TriangleMesh::HeIndx nextHE;
        BLAengine::TriangleMesh::HeIndx oppositeHE;

    private:
        friend class cereal::access;

        template <class Archive>
        void serialize(Archive & archive)
        {
            archive
            (
                destVertex, borderingFace, nextHE, oppositeHE
            );
        }
    };

    class RenderDataSerializer
    {
    public:

        RenderDataSerializer() = default;

        std::vector<unsigned int> triangleIndices;
        std::vector<vec3serializer> vertPos;
        std::vector<vec3serializer> vertNormal;
        std::vector<vec3serializer> vertTangent;
        std::vector<vec3serializer> vertBiTangent;
        std::vector<vec2serializer> vertUVs;

    private:
        friend class cereal::access;

        template <class Archive>
        void serialize(Archive & archive)
        {
            archive
            (
                triangleIndices, vertPos, vertNormal, vertTangent, vertBiTangent, vertUVs
            );
        }
    };
        
    TriangleMeshSerializer() = default;

    void vec3VectorToSerializer(std::vector<vec3> &actualVector, std::vector<vec3serializer> &serialVector);
    void vec2VectorToSerializer(std::vector<vec2> &actualVector, std::vector<vec2serializer> &serialVector);
    void vec3SerializerVectorToVec3(std::vector<vec3> &actualVector, std::vector<vec3serializer> &serialVector);
    void vec2SerializerVectorToVec2(std::vector<vec2> &actualVector, std::vector<vec2serializer> &serialVector);

    void BuildFromMesh(BLAengine::TriangleMesh* triangleMesh);
    BLAengine::TriangleMesh* BuildMesh();

private:

    friend class cereal::access;

    std::string m_name;
        
    std::vector<FaceSerializer> m_meshTriangles;
    std::vector<HalfEdgeSerializer> m_halfEdges;
    std::vector<unsigned int> m_heEmanatingFromVert;
    std::vector<vec3serializer> m_vertexPos;
    std::vector<vec3serializer> m_vertexNormals;
    std::vector<vec2serializer> m_vertexUVs;
    std::vector<vec3serializer> m_faceTangent;

    RenderDataSerializer m_renderData;

    int m_manifoldViolationEdges = 0;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            m_name,
            m_meshTriangles,
            m_halfEdges,
            m_heEmanatingFromVert,
            m_vertexPos,
            m_vertexNormals,
            m_vertexUVs,
            m_faceTangent,
            m_manifoldViolationEdges,
            m_renderData
        );
    }
};