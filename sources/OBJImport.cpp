/*Author: Vingt-2
	BLAengine
*/

#include "OBJImport.h"


OBJImport::OBJImport(void)
{
	meshVertices	= std::vector<Vector3f>();
	meshFaces		= std::vector<Vector3f>();
	texCoords		= std::vector<Vector3f>();
}


OBJImport::~OBJImport(void)
{
} 

bool OBJImport::ImportMesh(string filename)
{
	cout << "Importing " << filename << ".\n";
	ifstream fileStream (filename,ifstream::in);
	string lineInFile = " ";

	// Pushing a zero vertice in the vertices because ObjImport starts counting faces at 1,
	// Keeping things consistent
	meshVertices.push_back(Vector3f());

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
					Vector3f currentVertice;
					sscanf(lineInFile.data(),"%*s %f %f %f",&currentVertice.x,&currentVertice.y,&currentVertice.z);
					meshVertices.push_back(currentVertice);
				}
				else if(lineInFile.at(0) == 'f')
				{
					Vector3f currentFace;
					sscanf(lineInFile.data(),"%*s %f %f %f",&currentFace.x,&currentFace.y,&currentFace.z);
					meshFaces.push_back(currentFace);
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
		cout << filename << " imported, " << meshVertices.size() << " vertices and " << meshFaces.size() << " faces.\n";
		//cout << uselessLines << " unused Lines\n";
		return true;
	}
	else
	{
		cout << "Failed to Import " << filename << ".\n";
		return false;
	}
}