#pragma once
#include "gameobject.h"
#include "../../Assets/PolygonalMesh.h"
#include "..\..\..\Common\StdInclude.h"

class BLACORE_API GameChar : public GameObject
{
public:
	MeshRenderer* m_meshRenderer;
	RigidBody* m_rigidBody;

	void Update();

	void SetTriangleMesh(TriangleMesh* mesh);

	GameChar();
	~GameChar(void);
};

