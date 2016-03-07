#pragma once
#include "gameobject.h"
class GameVisible : public GameObject
{
public:
	MeshRenderer* m_meshRenderer;

	void Update();

	GameVisible(void);
	~GameVisible(void);
};
