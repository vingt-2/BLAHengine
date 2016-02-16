#pragma once
#include "GameObject.h"
#include "GameManager.h"

class GameManager : public GameObject
{
public:

	GameManager(void);
	~GameManager(void);

	void Update();
};

