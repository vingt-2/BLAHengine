#pragma once
#include "gameobject.h"
#include "..\..\..\Common\StdInclude.h"

class BLACORE_API GameVisible : public GameObject
{
public:
	MeshRenderer* m_meshRenderer;

	void Update();

	GameVisible(void);
	~GameVisible(void);
};
