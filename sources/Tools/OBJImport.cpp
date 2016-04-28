/*Author: Vingt-2
BLAengine
*/

#include "OBJImport.h"


OBJImport::OBJImport(void):
	m_currentMaxVertexPos(0),
	m_currentMaxUVPos(0),
	m_currentMaxNormalPos(0)
{
}


OBJImport::~OBJImport(void)
{
} 

typedef struct vertEntry{
	vec3 vx;
	vec3 vn;
	vec2 vt;

	bool operator==(const vertEntry& other) const
	{
		float epsilon = 0.00000000000001;

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
} ve_t;

struct vertEntryHasher
{
	std::size_t operator()(const vertEntry& v) const
	{
		int p1 = 73856093;
		int p2 = 19349663;
		int p3 = 83492791;

		int hashVx = (int)(100 * v.vx.x)*p1 ^ (int)(100 * v.vx.y)*p2 ^ (int)(100 * v.vx.z)*p3;
		int hashVn = (int)(100 * v.vn.x)*p1 ^ (int)(100 * v.vn.y)*p2 ^ (int)(100 * v.vn.z)*p3;
		int hashVt = (int)(100 * v.vt.x)*p1 ^ (int)(100 * v.vt.y)*p2;

		return hashVx + hashVn + hashVt;
	}
};

bool OBJImport::ImportMesh(const string filename, MeshAsset *mesh, bool swapNormals)
{

	m_currentMaxVertexPos = 0;
	m_currentMaxUVPos = 0;
	m_currentMaxNormalPos = 0;
	cout << "[OBJ_MESH] Importing " << filename << ".\n";
	ifstream fileStream (filename,ifstream::in);
	string lineInFile = " ";

	vector<unsigned int> meshTriangles;

	vector<vec3> collectedVertices;
	vector<vec2> collectedUVs;
	vector<vec3> collectedNormals;
	vector<int> vertexIndices, uvIndices, normalIndices;


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
							collectedVertices.push_back(currentVertice);
						}
						else if(lineInFile.at(1) == 't')
						{
							m_currentMaxUVPos ++;
							vec2 currentTexCoord;
							sscanf(lineInFile.data(),"%*s %f %f",&currentTexCoord.x,&currentTexCoord.y);
							collectedUVs.push_back(currentTexCoord);
						}
						else if(lineInFile.at(1) == 'n')
						{
							m_currentMaxNormalPos ++;
							vec3 currentNormal;
							sscanf(lineInFile.data(), "%*s %f %f %f\n", &currentNormal.x, &currentNormal.y, &currentNormal.z );
							collectedNormals.push_back(currentNormal);

						}
					}
					else if(lineInFile.at(0) == 'f')
					{
						if(lineInFile.at(1) == ' ')
						{
							int vertexIndex[3], uvIndex[3], normalIndex[3];
							if(sscanf(lineInFile.data(),"%*s %d/%d/%d %d/%d/%d %d/%d/%d\n",&vertexIndex[0],&uvIndex[0],&normalIndex[0]
							,&vertexIndex[1],&uvIndex[1],&normalIndex[1]
							,&vertexIndex[2],&uvIndex[2],&normalIndex[2]) == 9 )
							{
								vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
								uvIndices.push_back(FindUVAtIndex(uvIndex[0])),uvIndices.push_back(FindUVAtIndex(uvIndex[1])),uvIndices.push_back(FindUVAtIndex(uvIndex[2]));
								normalIndices.push_back(FindNormalAtIndex(normalIndex[0])),normalIndices.push_back(FindNormalAtIndex(normalIndex[1])),normalIndices.push_back(FindNormalAtIndex(normalIndex[2]));
							}
							else if (sscanf(lineInFile.data(), "%*s %d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]) == 3)
							{
								vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])), vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
							}
							else if(sscanf(lineInFile.data(),"%*s %d//%d %d//%d %d//%d\n",&vertexIndex[0],&normalIndex[0]
							,&vertexIndex[1],&normalIndex[1]
							,&vertexIndex[2],&normalIndex[2]) == 6 )
							{
								vertexIndices.push_back(FindVertexAtIndex(vertexIndex[0])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[1])),vertexIndices.push_back(FindVertexAtIndex(vertexIndex[2]));
								normalIndices.push_back(FindUVAtIndex(normalIndex[0])),normalIndices.push_back(FindUVAtIndex(normalIndex[1])),normalIndices.push_back(FindUVAtIndex(normalIndex[2]));
							}

						}
					}
				}
				else
				{
					uselessLines++;
				}
			}
			catch (out_of_range& outOfRangeException){}
		}
		fileStream.close();

		int vertexInd = 0 ;
		int uvInd = 0;
		int normalInd = 0;

		
		vector<vec3> meshVertices;
		vector<vec3> meshNormals;
		vector<vec2> meshUVs;

		unordered_map<vertEntry, int, vertEntryHasher> storedTriplets;

		int cacheMiss = 0;
		int cacheHit = 0;

		for (int index = 0; index < vertexIndices.size(); index += 3)
		{
			vertEntry v1, v2, v3;

			v1.vx = collectedVertices[vertexIndices[index]];
			v2.vx = collectedVertices[vertexIndices[index + 1]];
			v3.vx = collectedVertices[vertexIndices[index + 2]];

			if (uvIndices.size() != 0)
			{
				v1.vt = index < uvIndices.size() ? collectedUVs[uvIndices[index]] : vec2(0, 0);
				v2.vt = index + 1 < uvIndices.size() ? collectedUVs[uvIndices[index + 1]] : vec2(0, 0);
				v3.vt = index + 2 < uvIndices.size() ? collectedUVs[uvIndices[index + 2]] : vec2(0, 0);
			}

			if (normalIndices.size() != 0)
			{
				v1.vn = collectedNormals[normalIndices[index]];
				v2.vn = collectedNormals[normalIndices[index + 1]];
				v3.vn = collectedNormals[normalIndices[index + 2]];
			}

			if (storedTriplets.count(v1) == 0)
			{
				cacheMiss++;
				int endArrayIndex = meshVertices.size();

				meshVertices.push_back(v1.vx);
				if (normalIndices.size() != 0) meshNormals.push_back(v1.vn);
				if (uvIndices.size() != 0) meshUVs.push_back(v1.vt);

				storedTriplets[v1] = endArrayIndex;
				meshTriangles.push_back(endArrayIndex);
			}
			else
			{

				cacheHit++;
				meshTriangles.push_back(storedTriplets[v1]);
			}

			if (storedTriplets.count(v2) == 0)
			{
				cacheMiss++;
				int endArrayIndex = meshVertices.size();

				meshVertices.push_back(v2.vx);
				if (normalIndices.size() != 0) meshNormals.push_back(v2.vn);
				if (uvIndices.size() != 0) meshUVs.push_back(v2.vt);

				storedTriplets[v2] = endArrayIndex;
				meshTriangles.push_back(endArrayIndex);
			}
			else
			{

				cacheHit++;
				meshTriangles.push_back(storedTriplets[v2]);
			}

			if (storedTriplets.count(v3) == 0)
			{
				cacheMiss++;
				int endArrayIndex = meshVertices.size();

				meshVertices.push_back(v3.vx);
				if (normalIndices.size() != 0) meshNormals.push_back(v3.vn);
				if (uvIndices.size() != 0) meshUVs.push_back(v3.vt);

				storedTriplets[v3] = endArrayIndex;
				meshTriangles.push_back(endArrayIndex);
			}
			else
			{

				cacheHit++;
				meshTriangles.push_back(storedTriplets[v3]);
			}

		}

		if (swapNormals)
		{
			for (int i = 0; i < meshTriangles.size(); i += 3)
			{
				unsigned int a, b, c;
				a = meshTriangles[i+0];
				b = meshTriangles[i+1];
				c = meshTriangles[i+2];

				meshTriangles[i + 2] = a;
				meshTriangles[i + 1] = b;
				meshTriangles[i + 0] = c;

			}
		}

		mesh->m_meshTriangles = meshTriangles;
		mesh->m_meshVertices  = meshVertices;
		mesh->m_meshNormals	  = meshNormals;
		mesh->m_meshUVs		  = meshUVs;

		cout << "Imported: " << meshTriangles.size() << " triangles, " << meshVertices.size() << " triplets, " << collectedVertices.size() << " vertices, " << collectedNormals.size() << " normals, " << collectedUVs.size() << " UVs.\n";

		mesh->NormalizeModelCoordinates();

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