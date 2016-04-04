#pragma once
#include "gameobject.h"
#include "../../AssetsManager/MeshAsset.h"
class GameChar : public GameObject
{
public:
	MeshRenderer* m_meshRenderer;
	RigidBody* m_rigidBody;

	void Update();

	GameChar(MeshAsset* mesh);
	~GameChar(void);
};

