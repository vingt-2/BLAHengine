#pragma once

/*Author: Vingt-2
	BLAengine
*/
#include "std.h"
#include "Helpers.h"
#include "GameObject.h"

class OBJImport
{
public:
	static bool OldImportMesh(char* filename);
	static bool ImportMesh(const string filename, MeshRenderer *mesh);

	//Constr/deconstr
	OBJImport(void);
	~OBJImport(void);

private: 

};

