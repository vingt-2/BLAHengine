#pragma once
#include "gameobject.h"
class GameChar : public GameObject
{
public:
	MeshRenderer* m_meshRenderer;
	RigidBody* m_rigidBody;

	void Update();

	GameChar(void);
	~GameChar(void);
};

