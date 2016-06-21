#pragma once
#include "../Common/std.h"
#include "../Engine/AssetsManager/PolygonalMesh.h"

class OBJImport
{
public:
	bool ImportMesh(const string filename, TriangleMesh& mesh, bool swapNormals);

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

