#include "MeshSerializer.h"
using namespace BLA;

void TriangleMeshSerializer::vec3VectorToSerializer(blaVector<blaVec3> &actualVector, blaVector<vec3serializer> &serialVector)
{
    serialVector.resize(actualVector.size());
    for (size_t i = 0; i < actualVector.size(); i++)
    {
        serialVector[i] = vec3serializer();
        serialVector[i].FillData(actualVector[i]);
    }
}

void TriangleMeshSerializer::vec2VectorToSerializer(blaVector<glm::vec2> &actualVector, blaVector<vec2serializer> &serialVector)
{
    serialVector.resize(actualVector.size());
    for (size_t i = 0; i < actualVector.size(); i++)
    {
        serialVector[i] = vec2serializer();
        serialVector[i].FillData(actualVector[i]);
    }
}

void TriangleMeshSerializer::vec3serializerVectorTovec3(blaVector<blaVec3> &actualVector, blaVector<vec3serializer> &serialVector)
{
    actualVector.resize(serialVector.size());
    for (size_t i = 0; i < actualVector.size(); i++)
    {
        actualVector[i] = blaVec3();
        serialVector[i].LoadData(actualVector[i]);
    }
}

void TriangleMeshSerializer::vec2serializerVectorTovec2(blaVector<glm::vec2> &actualVector, blaVector<vec2serializer> &serialVector)
{
    actualVector.resize(serialVector.size());
    for (size_t i = 0; i < actualVector.size(); i++)
    {
        actualVector[i] = glm::vec2();
        serialVector[i].LoadData(actualVector[i]);
    }
}

void TriangleMeshSerializer::BuildFromMesh(MeshAsset* meshAsset)
{
    m_name = meshAsset->GetName();

    TriangleMesh* triangleMesh = &meshAsset->m_triangleMesh;

    m_meshTriangles.resize(triangleMesh->m_meshTriangles.size());

    for (size_t i = 0; i < triangleMesh->m_meshTriangles.size(); i++)
    {
        m_meshTriangles[i] = FaceSerializer();
        m_meshTriangles[i].FillData(triangleMesh->m_meshTriangles[i]);
    }

    m_halfEdges.resize(triangleMesh->m_halfEdges.size());

    for (size_t i = 0; i < triangleMesh->m_halfEdges.size(); i++)
    {
        m_halfEdges[i] = HalfEdgeSerializer();
        m_halfEdges[i].FillData(triangleMesh->m_halfEdges[i]);
    }

    m_heEmanatingFromVert = triangleMesh->m_heEmanatingFromVert;
    vec3VectorToSerializer(triangleMesh->m_vertexPos, m_vertexPos);
    vec3VectorToSerializer(triangleMesh->m_vertexNormals, m_vertexNormals);
    vec2VectorToSerializer(triangleMesh->m_vertexUVs, m_vertexUVs);
    vec3VectorToSerializer(triangleMesh->m_faceTangents, m_faceTangent);
    m_manifoldViolationEdges = triangleMesh->m_manifoldViolationEdges;

    m_renderData.triangleIndices = triangleMesh->m_renderData.m_triangleIndices;
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertPos, m_renderData.vertPos);
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertNormal, m_renderData.vertNormal);
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertTangent, m_renderData.vertTangent);
    vec3VectorToSerializer(triangleMesh->m_renderData.m_vertBiTangent, m_renderData.vertBiTangent);
    vec2VectorToSerializer(triangleMesh->m_renderData.m_vertUVs, m_renderData.vertUVs);

	for(const auto& m : triangleMesh->m_materials)
	{
        m_materials.push_back(m.first);
        m_materialIndex.push_back(m.second);
	}
}

MeshAsset* TriangleMeshSerializer::BuildMesh()
{
    MeshAsset* meshAsset = new MeshAsset(m_name);

    TriangleMesh* triangleMesh = &(meshAsset->m_triangleMesh);

    triangleMesh->m_meshTriangles.resize(m_meshTriangles.size());

    for (size_t i = 0; i < triangleMesh->m_meshTriangles.size(); i++)
    {
        triangleMesh->m_meshTriangles[i] = TriangleMesh::Face();
        m_meshTriangles[i].LoadData(triangleMesh->m_meshTriangles[i]);
    }

    triangleMesh->m_halfEdges.resize(m_halfEdges.size());

    for (size_t i = 0; i < triangleMesh->m_halfEdges.size(); i++)
    {
        triangleMesh->m_halfEdges[i] = TriangleMesh::HalfEdge();
        m_halfEdges[i].LoadData(triangleMesh->m_halfEdges[i]);
    }

    triangleMesh->m_heEmanatingFromVert = m_heEmanatingFromVert;
    vec3serializerVectorTovec3(triangleMesh->m_vertexPos, m_vertexPos);
    vec3serializerVectorTovec3(triangleMesh->m_vertexNormals, m_vertexNormals);
    vec2serializerVectorTovec2(triangleMesh->m_vertexUVs, m_vertexUVs);
    vec3serializerVectorTovec3(triangleMesh->m_faceTangents, m_faceTangent);

    triangleMesh->m_manifoldViolationEdges = m_manifoldViolationEdges;

    triangleMesh->m_renderData.m_triangleIndices = m_renderData.triangleIndices;
    vec3serializerVectorTovec3(triangleMesh->m_renderData.m_vertPos, m_renderData.vertPos);
    vec3serializerVectorTovec3(triangleMesh->m_renderData.m_vertNormal, m_renderData.vertNormal);
    vec3serializerVectorTovec3(triangleMesh->m_renderData.m_vertTangent, m_renderData.vertTangent);
    vec3serializerVectorTovec3(triangleMesh->m_renderData.m_vertBiTangent, m_renderData.vertBiTangent);
    vec2serializerVectorTovec2(triangleMesh->m_renderData.m_vertUVs, m_renderData.vertUVs);

    for (int i = 0; i < m_materials.size(); ++i)
    {
        triangleMesh->m_materials.push_back(std::make_pair(m_materials[i], m_materialIndex[i]));
    }

    return meshAsset;
}