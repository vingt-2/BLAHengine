#include "MeshSerializer.h"
using namespace BLAengine;

void TriangleMeshSerializer::vec3VectorToSerializer(std::vector<vec3> &actualVector, std::vector<vec3serializer> &serialVector)
{
    serialVector.resize(actualVector.size());
    for (int i = 0; i < actualVector.size(); i++)
    {
        serialVector[i] = vec3serializer();
        serialVector[i].FillData(actualVector[i]);
    }
}

void TriangleMeshSerializer::vec2VectorToSerializer(std::vector<vec2> &actualVector, std::vector<vec2serializer> &serialVector)
{
    serialVector.resize(actualVector.size());
    for (int i = 0; i < actualVector.size(); i++)
    {
        serialVector[i] = vec2serializer();
        serialVector[i].FillData(actualVector[i]);
    }
}

void TriangleMeshSerializer::vec3SerializerVectorToVec3(std::vector<vec3> &actualVector, std::vector<vec3serializer> &serialVector)
{
    actualVector.resize(serialVector.size());
    for (int i = 0; i < actualVector.size(); i++)
    {
        actualVector[i] = vec3();
        serialVector[i].LoadData(actualVector[i]);
    }
}

void TriangleMeshSerializer::vec2SerializerVectorToVec2(std::vector<vec2> &actualVector, std::vector<vec2serializer> &serialVector)
{
    actualVector.resize(serialVector.size());
    for (int i = 0; i < actualVector.size(); i++)
    {
        actualVector[i] = vec2();
        serialVector[i].LoadData(actualVector[i]);
    }
}

void TriangleMeshSerializer::BuildFromMesh(TriangleMesh* triangleMesh)
{
    m_name = triangleMesh->GetName();

    m_meshTriangles.resize(triangleMesh->m_meshTriangles.size());

    for (int i = 0; i < triangleMesh->m_meshTriangles.size(); i++)
    {
        m_meshTriangles[i] = FaceSerializer();
        m_meshTriangles[i].FillData(triangleMesh->m_meshTriangles[i]);
    }

    m_halfEdges.resize(triangleMesh->m_halfEdges.size());

    for (int i = 0; i < triangleMesh->m_halfEdges.size(); i++)
    {
        m_halfEdges[i] = HalfEdgeSerializer();
        m_halfEdges[i].FillData(triangleMesh->m_halfEdges[i]);
    }

    m_heEmanatingFromVert = triangleMesh->m_heEmanatingFromVert;
    vec3VectorToSerializer(triangleMesh->m_vertexPos, m_vertexPos);
    vec3VectorToSerializer(triangleMesh->m_vertexNormals, m_vertexNormals);
    vec2VectorToSerializer(triangleMesh->m_vertexUVs, m_vertexUVs);
    vec3VectorToSerializer(triangleMesh->m_faceTangent, m_faceTangent);
    m_manifoldViolationEdges = triangleMesh->m_manifoldViolationEdges;

    m_renderData.triangleIndices = triangleMesh->m_renderData.m_triangleIndices;
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertPos, m_renderData.vertPos);
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertNormal, m_renderData.vertNormal);
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertTangent, m_renderData.vertTangent);
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertBiTangent, m_renderData.vertBiTangent);
    vec2VectorToSerializer(triangleMesh->m_renderData.m_vertUVs, m_renderData.vertUVs);
}

TriangleMesh* TriangleMeshSerializer::BuildMesh()
{
    TriangleMesh* triangleMesh = new TriangleMesh(m_name);

    triangleMesh->m_meshTriangles.resize(m_meshTriangles.size());

    for (int i = 0; i < triangleMesh->m_meshTriangles.size(); i++)
    {
        triangleMesh->m_meshTriangles[i] = TriangleMesh::Face();
        m_meshTriangles[i].LoadData(triangleMesh->m_meshTriangles[i]);
    }

    triangleMesh->m_halfEdges.resize(m_halfEdges.size());

    for (int i = 0; i < triangleMesh->m_halfEdges.size(); i++)
    {
        triangleMesh->m_halfEdges[i] = TriangleMesh::HalfEdge();
        m_halfEdges[i].LoadData(triangleMesh->m_halfEdges[i]);
    }

    triangleMesh->m_heEmanatingFromVert = m_heEmanatingFromVert;
    vec3SerializerVectorToVec3(triangleMesh->m_vertexPos, m_vertexPos);
    vec3SerializerVectorToVec3(triangleMesh->m_vertexNormals, m_vertexNormals);
    vec2SerializerVectorToVec2(triangleMesh->m_vertexUVs, m_vertexUVs);
    vec3SerializerVectorToVec3(triangleMesh->m_faceTangent, m_faceTangent);

    triangleMesh->m_manifoldViolationEdges = m_manifoldViolationEdges;

    triangleMesh->m_renderData.m_triangleIndices = m_renderData.triangleIndices;
    vec3SerializerVectorToVec3(triangleMesh->m_renderData.m_vertPos, m_renderData.vertPos);
    vec3SerializerVectorToVec3(triangleMesh->m_renderData.m_vertNormal, m_renderData.vertNormal);
    vec3SerializerVectorToVec3(triangleMesh->m_renderData.m_vertTangent, m_renderData.vertTangent);
    vec3SerializerVectorToVec3(triangleMesh->m_renderData.m_vertBiTangent, m_renderData.vertBiTangent);
    vec2SerializerVectorToVec2(triangleMesh->m_renderData.m_vertUVs, m_renderData.vertUVs);

    return triangleMesh;
}