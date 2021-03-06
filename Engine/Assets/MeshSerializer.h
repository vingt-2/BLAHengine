// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "cereal\cereal.hpp"
#include "cereal\types\vector.hpp"
#include "cereal\types\string.hpp"
#include "cereal\archives\binary.hpp"

#include "Maths/Maths.h"
#include "Assets/MeshAsset.h"

#include "MathSerializer.h"

class TriangleMeshSerializer
{
public:
    class FaceSerializer
    {
    public:
        FaceSerializer() = default;

        void FillData(BLA::TriangleMesh::Face f)
        {
            firstEdge = f.firstEdge;
        }

        void LoadData(BLA::TriangleMesh::Face &f)
        {
            f.firstEdge = firstEdge;
        }

    private:
        BLA::TriangleMesh::HeIndx firstEdge;

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

        void FillData(BLA::TriangleMesh::DestVertex dvert)
        {
            pos = dvert.posIndex;
            normal = dvert.normalIndex;
            UV = dvert.uvIndex;
        }

        void LoadData(BLA::TriangleMesh::DestVertex &dvert)
        {
            dvert.posIndex = pos;
            dvert.normalIndex = normal;
            dvert.uvIndex = UV;
        }

        blaU32 pos; // Vertex the HE points to.
        blaU32 normal; // Normals at the destination Vertex (may be unique to this face)
        blaU32 UV; // UV at the destination Vertex (may be unique to this face)

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

        void FillData(BLA::TriangleMesh::HalfEdge he)
        {
            destVertex = DestVertexSerializer();
            destVertex.FillData(he.destVertex);

            borderingFace = he.borderingFace;
            nextHE = he.nextHE;
            oppositeHE = he.oppositeHE;
        }

        void LoadData(BLA::TriangleMesh::HalfEdge &he)
        {
            destVertex.LoadData(he.destVertex);
            he.borderingFace = borderingFace;
            he.nextHE = nextHE;
            he.oppositeHE = oppositeHE;
        }

        DestVertexSerializer destVertex;
        BLA::TriangleMesh::FaceIndx borderingFace;
        BLA::TriangleMesh::HeIndx nextHE;
        BLA::TriangleMesh::HeIndx oppositeHE;

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

        blaVector<blaU32> triangleIndices;
        blaVector<vec3serializer> vertPos;
        blaVector<vec3serializer> vertNormal;
        blaVector<vec3serializer> vertTangent;
        blaVector<vec3serializer> vertBiTangent;
        blaVector<vec2serializer> vertUVs;

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

    void vec3VectorToSerializer(blaVector<blaVec3> &actualVector, blaVector<vec3serializer> &serialVector);
    void vec2VectorToSerializer(blaVector<glm::vec2> &actualVector, blaVector<vec2serializer> &serialVector);
    void vec3serializerVectorTovec3(blaVector<blaVec3> &actualVector, blaVector<vec3serializer> &serialVector);
    void vec2serializerVectorTovec2(blaVector<glm::vec2> &actualVector, blaVector<vec2serializer> &serialVector);

    void BuildFromMesh(BLA::MeshAsset* triangleMesh);
    BLA::MeshAsset* BuildMesh();

private:

    friend class cereal::access;

    blaString m_name;

    blaVector<FaceSerializer> m_meshTriangles;
    blaVector<HalfEdgeSerializer> m_halfEdges;
    blaVector<blaU32> m_heEmanatingFromVert;
    blaVector<vec3serializer> m_vertexPos;
    blaVector<vec3serializer> m_vertexNormals;
    blaVector<vec2serializer> m_vertexUVs;
    blaVector<vec3serializer> m_faceTangent;
    blaVector<blaString> m_materials;
    blaVector<blaSize> m_materialIndex;

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
            m_renderData,
            m_materials,
            m_materialIndex
        );
    }
};