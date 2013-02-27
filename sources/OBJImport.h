/*Author: Vingt-2
	BLAengine
*/
#include "std.h"
#include "Helpers.h"
#include "MeshRenderer.h"

class OBJImport
{
public:
	bool OldImportMesh(char* filename);
	bool ImportMesh(const string filename, MeshRenderer *mesh);

	//Constr/deconstr
	OBJImport(void);
	~OBJImport(void);

private: 

};

