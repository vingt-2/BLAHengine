#include "OBJImport.h"

OBJImport::OBJImport(void) :
	m_currentMaxVertexPos(0),
	m_currentMaxUVPos(0),
	m_currentMaxNormalPos(0)
{
}


OBJImport::~OBJImport(void)
{
} 

bool OBJImport::ImportMesh(const string filename, TriangleMesh& mesh, bool swapNormals)
{

	m_currentMaxVertexPos = 0;
	m_currentMaxUVPos = 0;
	m_currentMaxNormalPos = 0;
	cout << "[OBJ_MESH] Importing " << filename << ".\n";
	ifstream fileStream (filename,ifstream::in);
	string lineInFile = " ";

	vector<int> vertexIndices, uvIndices, normalIndices;

	int quadsCount = 0;
	int missedFaces = 0;
	int triCount = 0;

	int outOfRangeExceptionsCount = 0;

	if(fileStream.good())
	{
		int uselessLines = 0;
		while(fileStream.good())
		{
			getline(fileStream,lineInFile);
			try
			{
				if(lineInFile.length() != 0)
				{
					if(lineInFile.at(0) == 'v')
					{
						if(lineInFile.at(1) == ' ')
						{
							m_currentMaxVertexPos ++;
							vec3 currentVertice;
							sscanf(lineInFile.data(),"%*s %f %f %f",&currentVertice.x,&currentVertice.y,&currentVertice.z);
							mesh.m_vertexPos.push_back(currentVertice);
						}
						else if(lineInFile.at(1) == 't')
						{
							m_currentMaxUVPos ++;
							vec2 currentTexCoord;
							sscanf(lineInFile.data(),"%*s %f %f",&currentTexCoord.x,&currentTexCoord.y);
							mesh.m_vertexUVs.push_back(currentTexCoord);
						}
						else if(lineInFile.at(1) == 'n')
						{
							m_currentMaxNormalPos ++;
							vec3 currentNormal;
							sscanf(lineInFile.data(), "%*s %f %f %f\n", &currentNormal.x, &currentNormal.y, &currentNormal.z );
							mesh.m_vertexNormals.push_back(currentNormal);
						}
					}
					else if(lineInFile.at(0) == 'f')
					{
						if(lineInFile.at(1) == ' ')
						{
							int vertexIndex[4], uvIndex[4], normalIndex[4];

							/*
								Starting off with the case where a face is defined as a quad.
								Simple triangulation is done.
							*/
							if (sscanf(lineInFile.data(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d%*s\n",
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
							else if (sscanf(lineInFile.data(), "%*s %d/%d %d/%d %d/%d %d/%d%*s\n",
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
							else if(sscanf(lineInFile.data(),"%*s %d/%d/%d %d/%d/%d %d/%d/%d%*s\n",
								&vertexIndex[0],&uvIndex[0],&normalIndex[0]
								,&vertexIndex[1],&uvIndex[1],&normalIndex[1]
								,&vertexIndex[2],&uvIndex[2],&normalIndex[2]) == 9 )
							{
								vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
								uvIndices.push_back(FindUVAtIndex(uvIndex[0])),uvIndices.push_back(FindUVAtIndex(uvIndex[1])),uvIndices.push_back(FindUVAtIndex(uvIndex[2]));
								normalIndices.push_back(FindNormalAtIndex(normalIndex[0])),normalIndices.push_back(FindNormalAtIndex(normalIndex[1])),normalIndices.push_back(FindNormalAtIndex(normalIndex[2]));
							
								triCount++;
							}
							else if (sscanf(lineInFile.data(), "%*s %d %d %d%*s\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]) == 3)
							{
								vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
								
								triCount++;
							}
							else if(sscanf(lineInFile.data(),"%*s %d//%d %d//%d %d//%d%*s\n",
								&vertexIndex[0],&normalIndex[0]
								,&vertexIndex[1],&normalIndex[1]
								,&vertexIndex[2],&normalIndex[2]) == 6 )
							{
								vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
								normalIndices.push_back(FindUVAtIndex(normalIndex[0])),normalIndices.push_back(FindUVAtIndex(normalIndex[1])),normalIndices.push_back(FindUVAtIndex(normalIndex[2]));
							
								triCount++;
							}
							else if (sscanf(lineInFile.data(), "%*s %d/%d %d/%d %d/%d%*s\n",
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
								missedFaces ++; 
							}
						}
					}
				}
				else
				{
					uselessLines++;
				}
			}
			catch (out_of_range& outOfRangeException)
			{
				outOfRangeExceptionsCount++;
			}
		}
		fileStream.close();

		int vertexInd = 0 ;
		int uvInd = 0;
		int normalInd = 0;

		unordered_map<pair<uint32_t, uint32_t>, uint32_t, uintPairHash> halfEdgesIndices; // an intermediate Data Structure to keep track of HEs
		vector<pair<uint32_t, uint32_t>> hePairs;

		mesh.m_heEmanatingFromVert.resize(vertexIndices.size());

		for (uint32_t i = 0; i < vertexIndices.size(); i += 3) // For each triangle
		{
			uint32_t currentTriangle = i / 3;

			uint32_t vindx0, vindx1, vindx2;
			uint32_t nindx0, nindx1, nindx2;
			uint32_t uvIndx0, uvIndx1, uvIndx2;

			if (swapNormals)
			{
				vindx0 = vertexIndices[i + 2];
				vindx1 = vertexIndices[i + 1];
				vindx2 = vertexIndices[i + 0];
			}
			else
			{
				vindx0 = vertexIndices[i + 1];
				vindx1 = vertexIndices[i + 2];
				vindx2 = vertexIndices[i + 0];
			}

			if (normalIndices.empty())
			{
				nindx0 = -1;
				nindx1 = -1;
				nindx2 = -1;
			}
			else
			{
				nindx0 = normalIndices[i + 1];
				nindx1 = normalIndices[i + 2];
				nindx2 = normalIndices[i + 0];
			}

			if (uvIndices.empty())
			{
				uvIndx0 = -1;
				uvIndx1 = -1;
				uvIndx2 = -1;
			}
			else
			{
				uvIndx0 = uvIndices[i + 1];
				uvIndx1 = uvIndices[i + 2];
				uvIndx2 = uvIndices[i + 0];
			}

			TriangleMesh::HalfEdge he0 = { vindx0, nindx0, uvIndx0, currentTriangle, i + 1, -1 }; // the -1 value is to be populated later
			TriangleMesh::HalfEdge he1 = { vindx1, nindx1, uvIndx1, currentTriangle, i + 2, -1 };
			TriangleMesh::HalfEdge he2 = { vindx2, nindx2, uvIndx2, currentTriangle, i + 0, -1 };

			mesh.m_halfEdges.push_back(he0);
			mesh.m_halfEdges.push_back(he1);
			mesh.m_halfEdges.push_back(he2);



			// Populating map and list to easily find opposite edges afterwards.
			uint32_t v0 = vertexIndices.at(i);
			uint32_t v1 = vertexIndices.at(i + 1);
			uint32_t v2 = vertexIndices.at(i + 2);
			halfEdgesIndices[pair<uint32_t, uint32_t>(v0, v1)] = i;
			halfEdgesIndices[pair<uint32_t, uint32_t>(v1, v2)] = i + 1;
			halfEdgesIndices[pair<uint32_t, uint32_t>(v2, v0)] = i + 2;
			hePairs.push_back(pair<uint32_t, uint32_t>(v0, v1));
			hePairs.push_back(pair<uint32_t, uint32_t>(v1, v2));
			hePairs.push_back(pair<uint32_t, uint32_t>(v2, v0));

			// Populates m_vertEmanatingHE
			mesh.m_heEmanatingFromVert[v0] = i;
			mesh.m_heEmanatingFromVert[v1] = i + 1;
			mesh.m_heEmanatingFromVert[v2] = i + 2;

			// Populates m_triangleHE
			TriangleMesh::face face = { i };
			mesh.m_meshTriangles.push_back(face);
		}

		for (uint32_t i = 0; i < mesh.m_halfEdges.size(); i++)
		{
			TriangleMesh::HalfEdge* edge = &(mesh.m_halfEdges.at(i));

			pair<uint32_t, uint32_t> vertPair = hePairs[i];

			try
			{
				edge->oppositeHE = halfEdgesIndices.at(pair<uint32_t, uint32_t>(vertPair.second, vertPair.first));
			}
			catch (const out_of_range& e)
			{
				edge->oppositeHE = 0xFFFFFFFF;
				mesh.m_manifoldViolationEdges++;
			}
		}

		if (swapNormals)
		{
			for (int i = 0; i < mesh.m_meshTriangles.size(); i += 3)
			{
				// TODO, reverse opposite edges
			}
		}

		cout << "Imported: " << mesh.m_meshTriangles.size() << " triangles, " << mesh.m_vertexPos.size() << " vertices. \n";

		mesh.NormalizeModelCoordinates();
		cout << "Normalized Model Coordinates \n";

		cout << " Built, improper edges: " << mesh.m_manifoldViolationEdges << "\n";

		return true;
	}
	else
	{
		cout << "Failed to Import " << filename << ".\n";
		return false;
	}
}

long OBJImport::FindVertexAtIndex(long index)
{
	if(index >= 0)
	{
		// .OBJ vertex indexing starts at 1 not 0, so substract 1 to each index
		return index - 1 ;
	}
	else
	{
		// .OBJ vertex Relative position. When negative value, the index references: ("total number of element of type X so far" - index)
		return m_currentMaxVertexPos+index;
	}
}
long OBJImport::FindUVAtIndex(long index)
{
	if(index >= 0)
	{
		// .OBJ vertex indexing starts at 1 not 0, so substract 1 to each index
		return index - 1;
	}
	else
	{
		// .OBJ vertex Relative position. When negative value, the index references: ("total number of element of type X so far" - index)
		return m_currentMaxUVPos+index;
	}
}
long OBJImport::FindNormalAtIndex(long index)
{
	if(index >= 0)
	{
		// .OBJ vertex indexing starts at 1 not 0, so substract 1 to each index
		return index - 1;
	}
	else
	{
		// .OBJ vertex Relative position. When negative value, the index references: ("total number of element of type X so far" - index)
		return m_currentMaxNormalPos+index;
	}
}