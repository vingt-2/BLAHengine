#pragma once
#include "../Std/std.h"
#include "../Helpers/Helpers.h"
#include "../Engine/Game/GameObjects/GameObject.h"
#include <unordered_map>

class OBJImport
{
public:
	static bool OldImportMesh(char* filename);

	bool ImportMesh(const string filename, MeshRenderer *mesh);

	OBJImport(void);
	~OBJImport(void);

private: 

	long m_currentMaxVertexPos;
	long m_currentMaxUVPos;
	long m_currentMaxNormalPos;

	long FindVertexAtIndex(long index);
	long FindUVAtIndex(long index);
	long FindNormalAtIndex(long index);
};

