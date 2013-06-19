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

bool OBJImport::ImportMesh(const string filename, MeshRenderer *mesh)
{
	vector<vec3> meshVertices;
	vector<vec3> meshNormals;
	vector<vec2> meshUVs;
	cout << "[OBJ_MESH] Importing " << filename << ".\n";
	ifstream fileStream (filename,ifstream::in);
	string lineInFile = " ";

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
			catch(out_of_range& outOfRangeException)
			{
				//	Non conventional shit is expected when parsing a file, just discard it.
			}
		}
		fileStream.close();
		
		cout << vertexIndices.size() << "+ " << uvIndices.size();

		int vertexInd = 0 ;
		int uvInd = 0;
		int normalInd = 0;

		for( int index =0 ; index<vertexIndices.size(); index++ )
		{
			//printf("%i \n.",index);
			vertexInd = vertexIndices[index];

			if( uvIndices.size() > 0) 
			{
				uvInd = uvIndices[index];
				vec2 uv		= collectedUVs[uvInd];
				meshUVs     .push_back(uv);
			}

			normalInd = normalIndices[index];

			vec3 vertex = collectedVertices[vertexInd];
			// UVS old place
			vec3 normal = collectedNormals[normalInd];

			meshVertices.push_back(vertex);
			// UVs old place
			meshNormals.push_back(normal);
		}

		mesh->m_meshVertices = meshVertices;
		mesh->m_meshNormals	 = meshNormals;
		mesh->m_meshUVs		 = meshUVs;

		//cout << uselessLines << " unused Lines\n";
		cout << "Imported: " << meshVertices.size() << " vertices, " << meshNormals.size() << " normals, " << meshUVs.size() << " UVs.\n";

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