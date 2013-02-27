/*Author: Vingt-2
BLAengine
*/

#include "OBJImport.h"


OBJImport::OBJImport(void)
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
	cout << "Importing " << filename << ".\n";
	ifstream fileStream (filename,ifstream::in);
	string lineInFile = " ";


	vector<vec3> collectedVertices;
	vector<vec2> collectedUVs;
	vector<vec3> collectedNormals;
	vector<int> vertexIndices, uvIndices, normalIndices;


	if(fileStream.good())
	{
		// And 1 useless counter, 1!
		int uselessLines = 0;
		while(fileStream.good())
		{
			getline(fileStream,lineInFile);
			try
			{
				if(lineInFile.at(0) == 'v')
				{
					if(lineInFile.at(1) == ' ')
					{
						vec3 currentVertice;
						sscanf(lineInFile.data(),"%*s %f %f %f",&currentVertice.x,&currentVertice.y,&currentVertice.z);
						collectedVertices.push_back(currentVertice);
					}
					else if(lineInFile.at(1) == 't')
					{
						vec2 currentTexCoord;
						sscanf(lineInFile.data(),"%*s %f %f",&currentTexCoord.x,&currentTexCoord.y);
						collectedUVs.push_back(currentTexCoord);
					}
					else if(lineInFile.at(1) == 'n')
					{
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
						sscanf(lineInFile.data(),"%*s %d/%d/%d %d/%d/%d %d/%d/%d\n",&vertexIndex[0],&uvIndex[0],&normalIndex[0]
						,&vertexIndex[1],&uvIndex[1],&normalIndex[1]
						,&vertexIndex[2],&uvIndex[2],&normalIndex[2]);

						vertexIndices.push_back(vertexIndex[0]),vertexIndices.push_back(vertexIndex[1]),vertexIndices.push_back(vertexIndex[2]);
						uvIndices.push_back(uvIndex[0]),uvIndices.push_back(uvIndex[1]),uvIndices.push_back(uvIndex[2]);
						normalIndices.push_back(normalIndex[0]),normalIndices.push_back(normalIndex[1]),normalIndices.push_back(normalIndex[2]);
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

		for( int i=0; i<vertexIndices.size(); i++ )
		{
			int vertexIndex = vertexIndices[i];
			int uvIndex = uvIndices[i];
			int normalIndex = normalIndices[i];

			// .OBJ vertex indexing starts at 1 not 0, so substract 1 to each index
			vec3 vertex = collectedVertices[vertexIndex-1];
			vec2 uv = collectedUVs[uvIndex-1];
			vec3 normal = collectedNormals[normalIndex-1];

			meshVertices.push_back(vertex);
			meshUVs     .push_back(uv);
			meshNormals .push_back(normal);
		}

		mesh->meshVertices	= meshVertices;
		mesh->meshNormals	= meshNormals;
		mesh->meshUVs		= meshUVs;

		//cout << uselessLines << " unused Lines\n";

		return true;
	}
	else
	{
		cout << "Failed to Import " << filename << ".\n";
		return false;
	}
}