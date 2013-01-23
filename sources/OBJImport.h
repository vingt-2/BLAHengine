/*Author: Vingt-2
	BLAengine
*/
#include "std.h"
#include "Vector3f.h"
#include "Helpers.h"

class OBJImport
{
public:
	std::vector<Vector3f> meshVertices;
	std::vector<Vector3f> meshFaces;
	std::vector<Vector3f> texCoords;

	bool OldImportMesh(char* filename);
	bool ImportMesh(string filename);

	//Constr/deconstr
	OBJImport(void);
	~OBJImport(void);

private: 

};

