#include "OBJImport.h"

using namespace BLA;

OBJImport::OBJImport(void) :
    m_currentMaxVertexPos(0),
    m_currentMaxUVPos(0),
    m_currentMaxNormalPos(0)
{}

bool OBJImport::ImportMesh(const blaString filename, TriangleMesh& mesh, bool swapNormals, bool normalScale)
{
    m_currentMaxVertexPos = 0;
    m_currentMaxUVPos = 0;
    m_currentMaxNormalPos = 0;
    std::cout << "[OBJ_MESH] Importing " << filename << ".\n";
    std::ifstream fileStream(filename, std::ifstream::in);
    blaString lineInFile = " ";

    blaVector<blaU32> vertexIndices, uvIndices, normalIndices;

    int quadsCount = 0;
    int missedFaces = 0;
    int triCount = 0;

    int outOfRangeExceptionsCount = 0;

    if (!fileStream.good())
    {
        std::cout << "Failed to Import " << filename << ".\n";
        return false;
    }

    int uselessLines = 0;
    while (fileStream.good())
    {
        getline(fileStream, lineInFile);
        try
        {
            if (lineInFile.length() == 0)
            {
                uselessLines++;
                continue;
            }

            if (lineInFile.at(0) == 'v')
            {
                if (lineInFile.at(1) == ' ')
                {
                    m_currentMaxVertexPos++;
                    blaVec3 currentVertice;
                    sscanf_s(lineInFile.data(), "%*s %f %f %f", &currentVertice.x, &currentVertice.y, &currentVertice.z);
                    mesh.m_vertexPos.push_back(currentVertice);
                }
                else if (lineInFile.at(1) == 't')
                {
                    m_currentMaxUVPos++;
                    glm::vec2 currentTexCoord;
                    sscanf_s(lineInFile.data(), "%*s %f %f", &currentTexCoord.x, &currentTexCoord.y);
                    mesh.m_vertexUVs.push_back(currentTexCoord);
                }
                else if (lineInFile.at(1) == 'n')
                {
                    m_currentMaxNormalPos++;
                    blaVec3 currentNormal;
                    sscanf_s(lineInFile.data(), "%*s %f %f %f\n", &currentNormal.x, &currentNormal.y, &currentNormal.z);
                    if (swapNormals)
                        currentNormal *= -1.f;
                    mesh.m_vertexNormals.push_back(currentNormal);
                }
            }
            else if (lineInFile.at(0) == 'f' && lineInFile.at(1) == ' ')
            {
                int vertexIndex[4], uvIndex[4], normalIndex[4];

                /*
                    Starting off with the case where a face is defined as a quad.
                    Simple triangulation is done.
                */
                if (sscanf_s(lineInFile.data(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d%*s\n",
                    &vertexIndex[0], &uvIndex[0], &normalIndex[0]
                    , &vertexIndex[1], &uvIndex[1], &normalIndex[1]
                    , &vertexIndex[2], &uvIndex[2], &normalIndex[2]
                    , &vertexIndex[3], &uvIndex[3], &normalIndex[3]) == 12)
                {
                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[3]));
                    uvIndices.push_back(FindUVAtIndex(uvIndex[0])), uvIndices.push_back(FindUVAtIndex(uvIndex[1])), uvIndices.push_back(FindUVAtIndex(uvIndex[3]));
                    normalIndices.push_back(FindNormalAtIndex(normalIndex[0])), normalIndices.push_back(FindNormalAtIndex(normalIndex[1])), normalIndices.push_back(FindNormalAtIndex(normalIndex[3]));

                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[3]));
                    uvIndices.push_back(FindUVAtIndex(uvIndex[1])), uvIndices.push_back(FindUVAtIndex(uvIndex[2])), uvIndices.push_back(FindUVAtIndex(uvIndex[3]));
                    normalIndices.push_back(FindNormalAtIndex(normalIndex[1])), normalIndices.push_back(FindNormalAtIndex(normalIndex[2])), normalIndices.push_back(FindNormalAtIndex(normalIndex[3]));

                    quadsCount++;
                }
                else if (sscanf_s(lineInFile.data(), "%*s %d/%d %d/%d %d/%d %d/%d%*s\n",
                    &vertexIndex[0], &uvIndex[0]
                    , &vertexIndex[1], &uvIndex[1]
                    , &vertexIndex[2], &uvIndex[2]
                    , &vertexIndex[3], &uvIndex[3]) == 8)
                {
                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[3]));
                    uvIndices.push_back(FindUVAtIndex(uvIndex[0])), uvIndices.push_back(FindUVAtIndex(uvIndex[1])), uvIndices.push_back(FindUVAtIndex(uvIndex[3]));

                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[3]));
                    uvIndices.push_back(FindUVAtIndex(uvIndex[1])), uvIndices.push_back(FindUVAtIndex(uvIndex[2])), uvIndices.push_back(FindUVAtIndex(uvIndex[3]));

                    quadsCount++;
                }
                else if (sscanf_s(lineInFile.data(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d%*s\n",
                    &vertexIndex[0], &uvIndex[0], &normalIndex[0]
                    , &vertexIndex[1], &uvIndex[1], &normalIndex[1]
                    , &vertexIndex[2], &uvIndex[2], &normalIndex[2]) == 9)
                {
                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
                    uvIndices.push_back(FindUVAtIndex(uvIndex[0])), uvIndices.push_back(FindUVAtIndex(uvIndex[1])), uvIndices.push_back(FindUVAtIndex(uvIndex[2]));
                    normalIndices.push_back(FindNormalAtIndex(normalIndex[0])), normalIndices.push_back(FindNormalAtIndex(normalIndex[1])), normalIndices.push_back(FindNormalAtIndex(normalIndex[2]));

                    triCount++;
                }
                else if (sscanf_s(lineInFile.data(), "%*s %d %d %d%*s\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]) == 3)
                {
                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));

                    triCount++;
                }
                else if (sscanf_s(lineInFile.data(), "%*s %d//%d %d//%d %d//%d%*s\n",
                    &vertexIndex[0], &normalIndex[0]
                    , &vertexIndex[1], &normalIndex[1]
                    , &vertexIndex[2], &normalIndex[2]) == 6)
                {
                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
                    normalIndices.push_back(FindUVAtIndex(normalIndex[0])), normalIndices.push_back(FindUVAtIndex(normalIndex[1])), normalIndices.push_back(FindUVAtIndex(normalIndex[2]));

                    triCount++;
                }
                else if (sscanf_s(lineInFile.data(), "%*s %d/%d %d/%d %d/%d%*s\n",
                    &vertexIndex[0], &uvIndex[0]
                    , &vertexIndex[1], &uvIndex[1]
                    , &vertexIndex[2], &uvIndex[2]) == 6)
                {
                    vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
                    uvIndices.push_back(FindUVAtIndex(uvIndex[0])), uvIndices.push_back(FindUVAtIndex(uvIndex[1])), uvIndices.push_back(FindUVAtIndex(uvIndex[2]));

                    triCount++;
                }
                else
                {
                    missedFaces++;
                }
            }
            else
            {
                uselessLines++;
            }
        }
        catch (std::out_of_range& outOfRangeException)
        {
            outOfRangeExceptionsCount++;
        }
    }
    fileStream.close();

    std::cout << "Done reading, building Topology... ";
    mesh.BuildMeshTopo(vertexIndices, normalIndices, uvIndices, swapNormals);
    std::cout << "Built, Computing Tangent Spaces...";
    mesh.ComputeFaceTangents();
    std::cout << "Done.\nImported: " << mesh.m_meshTriangles.size() << " triangles, " << mesh.m_vertexPos.size() << " vertices, " << mesh.m_manifoldViolationEdges << " non-manifold edges\n";
    std::cout << "Normalizing Model Coordinates \n";
    mesh.NormalizeModelCoordinates(normalScale);
    std::cout << "Generating Render Data\n";
    mesh.GenerateRenderData();

    return true;
}

long OBJImport::FindVertexAtIndex(long index)
{
    if (index >= 0)
    {
        // .OBJ vertex indexing starts at 1 not 0, so substract 1 to each index
        return index - 1;
    }
    else
    {
        // .OBJ vertex Relative position. When negative value, the index references: ("total number of element of type X so far" - index)
        return m_currentMaxVertexPos + index;
    }
}
long OBJImport::FindUVAtIndex(long index)
{
    if (index >= 0)
    {
        // .OBJ vertex indexing starts at 1 not 0, so substract 1 to each index
        return index - 1;
    }
    else
    {
        // .OBJ vertex Relative position. When negative value, the index references: ("total number of element of type X so far" - index)
        return m_currentMaxUVPos + index;
    }
}
long OBJImport::FindNormalAtIndex(long index)
{
    if (index >= 0)
    {
        // .OBJ vertex indexing starts at 1 not 0, so substract 1 to each index
        return index - 1;
    }
    else
    {
        // .OBJ vertex Relative position. When negative value, the index references: ("total number of element of type X so far" - index)
        return m_currentMaxNormalPos + index;
    }
}