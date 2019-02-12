#include "TriangleMesh.h"

using namespace BLAengine;

#define INVALID_HE 0xFFFFFFFF

#pragma optimize("", off)

void TriangleMesh::BuildMeshTopo(
    vector<blaU32> vertPosIndices,
    vector<blaU32> vertNormalIndices,
    vector<blaU32> vertUVsIndices,
    bool swapNormals = false)
{
    BLA_ASSERT(vertPosIndices.size() == vertNormalIndices.size() && vertNormalIndices.size() == vertUVsIndices.size())

    unordered_map<pair<blaU32, blaU32>, blaU32, uintPairHash> halfEdgesIndices; // an intermediate Data Structure to keep track of HEs
    vector<pair<blaU32, blaU32>> hePairs;

    m_heEmanatingFromVert.resize(vertPosIndices.size());

    for (blaU32 i = 0; i < vertPosIndices.size(); i += 3) // For each triangle
    {
        blaU32 currentTriangle = i / 3;

        blaU32 vindx0, vindx1, vindx2;
        blaU32 nindx0, nindx1, nindx2;
        blaU32 uvIndx0, uvIndx1, uvIndx2;

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

        HalfEdge he0 = { { vindx0, nindx0, uvIndx0 }, currentTriangle, i + 1, -1 }; // the -1 value is to be populated later
        HalfEdge he1 = { { vindx1, nindx1, uvIndx1 }, currentTriangle, i + 2, -1 };
        HalfEdge he2 = { { vindx2, nindx2, uvIndx2 }, currentTriangle, i + 0, -1 };

        m_halfEdges.push_back(he0);
        m_halfEdges.push_back(he1);
        m_halfEdges.push_back(he2);


        // Populating map and list to easily find opposite edges afterwards.
        blaU32 v0, v1, v2;

        if (swapNormals)
        {
            v0 = vertPosIndices.at(i + 2);
            v1 = vertPosIndices.at(i + 1);
            v2 = vertPosIndices.at(i + 0);
        }
        else
        {
            v0 = vertPosIndices.at(i);
            v1 = vertPosIndices.at(i + 1);
            v2 = vertPosIndices.at(i + 2);
        }

        halfEdgesIndices[pair<blaU32, blaU32>(v0, v1)] = i;
        halfEdgesIndices[pair<blaU32, blaU32>(v1, v2)] = i + 1;
        halfEdgesIndices[pair<blaU32, blaU32>(v2, v0)] = i + 2;
        hePairs.emplace_back(pair<blaU32, blaU32>(v0, v1));
        hePairs.emplace_back(pair<blaU32, blaU32>(v1, v2));
        hePairs.emplace_back(pair<blaU32, blaU32>(v2, v0));

        // Populates m_vertEmanatingHE
        m_heEmanatingFromVert[v0] = i;
        m_heEmanatingFromVert[v1] = i + 1;
        m_heEmanatingFromVert[v2] = i + 2;

        // Populates m_triangleHE
        Face face = { i };
        m_meshTriangles.push_back(face);
    }

    for (blaU32 i = 0; i < m_halfEdges.size(); i++)
    {
        HalfEdge* edge = &(m_halfEdges.at(i));

        pair<blaU32, blaU32> vertPair = hePairs[i];
        pair<blaU32, blaU32> invertVertPair = pair<blaU32, blaU32>(vertPair.second, vertPair.first);
        if (halfEdgesIndices.count(invertVertPair))
        {
            edge->oppositeHE = halfEdgesIndices[invertVertPair];
        }
        else
        {
            edge->oppositeHE = INVALID_HE;
            m_manifoldViolationEdges++;
        }
    }
}

void TriangleMesh::NormalizeModelCoordinates(bool normalizeScale)
{
    float maxDistance = 0;

    blaVec3 centroid = blaVec3(0, 0, 0);

    for (auto v : this->m_vertexPos)
    {
        centroid += v;
    }

    centroid /= m_vertexPos.size();

    for (blaU32 i = 0; i < m_vertexPos.size(); i++)
    {
        m_vertexPos[i] -= centroid;
    }
    
    if (normalizeScale)
    {
        for (auto v : this->m_vertexPos)
        {
            float distance = length(v);
            maxDistance = distance > maxDistance ? distance : maxDistance;
        }

        for (blaU32 i = 0; i < m_vertexPos.size(); i++)
        {
            m_vertexPos[i] /= maxDistance;
        }
    }
}

void TriangleMesh::ComputeFaceTangents()
{
    for (auto face : m_meshTriangles)
    {
        const HalfEdge& edge0 = m_halfEdges[face.firstEdge];
        const HalfEdge& edge1 = m_halfEdges[edge0.nextHE];
        const HalfEdge& edge2 = m_halfEdges[edge1.nextHE];

        const DestVertex& v0 = edge2.destVertex;
        const DestVertex& v1 = edge0.destVertex;
        const DestVertex& v2 = edge1.destVertex;

        blaVec3 q1 = m_vertexPos[v1.pos] - m_vertexPos[v0.pos];
        blaVec3 q2 = m_vertexPos[v2.pos] - m_vertexPos[v0.pos];

        blaVec2 st1 = m_vertexUVs[v1.UV] - m_vertexUVs[v0.UV];
        blaVec2 st2 = m_vertexUVs[v2.UV] - m_vertexUVs[v0.UV];

        float invScale = 1 / (st1.s*st2.t - st2.s*st1.t);

        if (isnan(invScale))
            invScale = 0.0f;
        
        blaVec3 T = invScale * glm::mat2x3(q1, q2) * glm::vec2(st2.t, -st1.t);
        //blaVec3 B = invScale * mat2x3(q1, q2) * glm::vec2(-st2.s, st1.s);

        if (isnan(T.x) || isnan(T.y) || isnan(T.z))
        {
            //BLA_ASSERT(false)
            T = blaVec3(0);
        }

        m_faceTangent.push_back(T);
    }
}

void TriangleMesh::ApplyGeomScaling(blaVec3 scaling)
{
    blaMat3 scaleMat(
        blaVec3(scaling.x, 0, 0),
        blaVec3(0, scaling.y, 0),
        blaVec3(0, 0, scaling.z));

    for (auto &v : m_vertexPos) 
    {
        v = scaleMat * v;
    }
}

void TriangleMesh::ApplyUVScaling(glm::vec2 scaling)
{
    glm::mat2 scaleMat(glm::vec2(scaling.x, 0), glm::vec2(0, scaling.y));

    for (auto &v : m_vertexUVs) 
    {
        v = scaleMat * v;
    }
}

void TriangleMesh::GenerateRenderData()
{
    unordered_map<RenderVertEntry, blaU32, vertEntryHasher> vertexMap;
    
    m_renderData.m_triangleIndices.resize(3*this->m_meshTriangles.size());

    for (blaU32 triIndx = 0; triIndx < m_meshTriangles.size(); triIndx ++)
    {
        HalfEdge edge0 = m_halfEdges[m_meshTriangles[triIndx].firstEdge];
        HalfEdge edge1 = m_halfEdges[edge0.nextHE];
        HalfEdge edge2 = m_halfEdges[edge1.nextHE];

        for (blaU32 i = 0; i < 3; i++)
        {
            HalfEdge edge;

            if (i == 0) edge = edge2;
            if (i == 1) edge = edge0;
            if (i == 2) edge = edge1;

            RenderVertEntry vert;

            vert.vx = m_vertexPos[edge.destVertex.pos];
            vert.vn = m_vertexNormals[edge.destVertex.normal];
            vert.vt = m_vertexUVs[edge.destVertex.UV];

            vector<FaceIndx> surroundingFaces;

            const bool correctQuery = GetSurroundingTriangles(edge.destVertex.pos, surroundingFaces);

            blaVec3 tangent = m_faceTangent[triIndx];
            
            if (correctQuery)
            {
                for (auto face : surroundingFaces)
                {
                    tangent += m_faceTangent[face];
                }
            }
            tangent = tangent - (vert.vn * glm::dot(tangent, vert.vn));
            
            if(length(tangent) > 0)
                tangent = normalize(tangent);

            if (isnan(tangent.x) || isnan(tangent.y) || isnan(tangent.z))
            {
                tangent = blaVec3(0);
            }

            if (vertexMap.count(vert) == 0)
            {
                m_renderData.m_vertPos.push_back(vert.vx);
                m_renderData.m_vertNormal.push_back(vert.vn);
                m_renderData.m_vertUVs.push_back(vert.vt);
                m_renderData.m_vertTangent.push_back(tangent);

                vertexMap[vert] = m_renderData.m_vertPos.size() - 1;

                m_renderData.m_triangleIndices[3 * triIndx + i] = m_renderData.m_vertPos.size() - 1;

            }
            else
            {
                m_renderData.m_triangleIndices[3 * triIndx + i] = vertexMap[vert];
            }
        }
    }
}

void TriangleMesh::GenerateTopoTriangleIndices(vector<blaU32> &posIndices, vector<blaU32> &normalIndices)
{
    for (size_t triIndx = 0; triIndx < m_meshTriangles.size(); triIndx++)
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

            posIndices.push_back(edge.destVertex.pos);
            normalIndices.push_back(edge.destVertex.normal);
        }
    }
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
    //    //blaU32 temp;

    //    //HalfEdge edge0 = m_halfEdges[face.firstEdge];
    //    //edge0.
    //    //HalfEdge edge1 = m_halfEdges[edge0.nextHE];
    //    //HalfEdge edge2 = m_halfEdges[edge1.nextHE];
    //}
}

void TriangleMesh::GetHEvertices(HeIndx halfEdge, pair<blaU32, blaU32>* vertexPair)
{
    HalfEdge edge = m_halfEdges[halfEdge];
    vertexPair->first = edge.destVertex.pos;
    
    if (edge.oppositeHE != INVALID_HE)
    {
        vertexPair->second = m_halfEdges[edge.oppositeHE].destVertex.pos;
    }
    else
    {
        // If no opposite HE exists, go around the existing face.
        HeIndx findEdgeIndx = edge.nextHE;
        HalfEdge findEdge = m_halfEdges[findEdgeIndx];
        do
        {
            findEdgeIndx = findEdge.nextHE;
        } while (m_halfEdges[findEdgeIndx].nextHE != halfEdge);

        vertexPair->second = m_halfEdges[findEdgeIndx].destVertex.pos;
    }
}

void TriangleMesh::GetHETriangle(HeIndx halfEdge, FaceIndx* triangle)
{
    *triangle = m_halfEdges[halfEdge].borderingFace;
}

bool TriangleMesh::GetSurroundingVertices(blaU32 vertexIndx, vector<DestVertex> &surroundingVertices)
{
    HeIndx startEdgeIndx = m_heEmanatingFromVert[vertexIndx];
    HeIndx currentEdgeIndx = startEdgeIndx;

    blaU32 failCount = 0;

    HalfEdge edge;
    do
    {
        edge = m_halfEdges[currentEdgeIndx];
        surroundingVertices.push_back(edge.destVertex);

        if (edge.oppositeHE != INVALID_HE)
        {
            currentEdgeIndx = m_halfEdges[edge.oppositeHE].nextHE;
        }
        else
        {
            failCount++;

            // Find inward edge along the same face (that we know exists), then take its opposite so it's out of the vertex
            HeIndx findEdgeIndx = edge.nextHE;
            HalfEdge findEdge = m_halfEdges[findEdgeIndx];
            do
            {
                findEdgeIndx = findEdge.nextHE;
            } while (m_halfEdges[findEdgeIndx].nextHE != currentEdgeIndx);

            HeIndx emanatingFromOriginalVertex = m_halfEdges[findEdgeIndx].oppositeHE;
            if (emanatingFromOriginalVertex == INVALID_HE)
            {
                // In that case the triangle is alone starting on that vertex.
                // Very degenerate case, just drop the query, it's worthless
                return false;
            }
            currentEdgeIndx = emanatingFromOriginalVertex;
        }
    } while (currentEdgeIndx != startEdgeIndx && failCount < 2);

    return true;
}

bool TriangleMesh::GetEmanatingHalfEdges(blaU32 vertexIndx, vector<HeIndx> &surroundingEdges)
{
    HeIndx startEdgeIndx = m_heEmanatingFromVert[vertexIndx];
    HeIndx currentEdgeIndx = startEdgeIndx;

    blaU32 failCount = 0;

    HalfEdge edge;
    do
    {
        edge = m_halfEdges[currentEdgeIndx];

        if (find(surroundingEdges.begin(), surroundingEdges.end(), currentEdgeIndx) != surroundingEdges.end())
        {
            // Cycle detected ...
            return false;
        }
        else
        {
            surroundingEdges.push_back(currentEdgeIndx);
        }
        
        if (edge.oppositeHE != INVALID_HE)
        {
            currentEdgeIndx = m_halfEdges[edge.oppositeHE].nextHE;
        }
        else
        {
            failCount++;

            // Find inward edge along the same face (that we know exists), then take it's opposite so it's out of the vertex
            HeIndx findEdgeIndx = edge.nextHE;
            HalfEdge findEdge = m_halfEdges[findEdgeIndx];
            do
            {
                findEdgeIndx = findEdge.nextHE;
            } while (m_halfEdges[findEdgeIndx].nextHE != currentEdgeIndx);

            HeIndx emanatingFromOriginalVertex = m_halfEdges[findEdgeIndx].oppositeHE;
            if (emanatingFromOriginalVertex == INVALID_HE)
            {
                // In that case the triangle is alone starting on that vertex.
                // Very degenerate case, just drop the query, it's worthless
                return false;
            }
            currentEdgeIndx = emanatingFromOriginalVertex;
        }
    } while (currentEdgeIndx != startEdgeIndx && failCount < 2);

    return true;
}

bool TriangleMesh::GetSurroundingTriangles(blaU32 vertexIndx, vector<FaceIndx> &surroundingFaces)
{
    vector<HeIndx> emanatingEdges;
    
    bool problem = !GetEmanatingHalfEdges(vertexIndx, emanatingEdges);

    for (auto edgeIndx : emanatingEdges)
    {
        HalfEdge edge = m_halfEdges[edgeIndx];
        surroundingFaces.push_back(edge.borderingFace);
    }
    
    return problem;
}

void TriangleMesh::GetTriangleEdges(blaU32 triangle, HeIndx* edge0, HeIndx* edge1, HeIndx* edge2)
{
    *edge0 = m_meshTriangles[triangle].firstEdge;
    *edge1 = m_halfEdges[*edge0].nextHE;
    *edge2 = m_halfEdges[*edge1].nextHE;
}