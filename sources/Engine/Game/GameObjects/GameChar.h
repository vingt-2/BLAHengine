#pragma once
#include "gameobject.h"
#include "../../AssetsManager/PolygonalMesh.h"
class GameChar : public GameObject
{
public:
	MeshRenderer* m_meshRenderer;
	RigidBody* m_rigidBody;

	void Update();

	GameChar(PolygonalMesh* mesh);
	~GameChar(void);
};

