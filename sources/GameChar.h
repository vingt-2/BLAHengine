#pragma once
#include "gameobject.h"
class GameChar : public GameObject
{
public:
	MeshRenderer* meshRenderer;
	RigidBody* rigidBody;

	void Update();

	GameChar(void);
	~GameChar(void);
};

