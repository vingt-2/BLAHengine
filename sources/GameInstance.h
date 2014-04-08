#pragma once
#include"RenderingManager.h"
#include"Engine\Renderer.h"
class GameInstance
{
public:

	RenderingManager* m_renderingManager;

	Renderer* m_renderer;

	GameInstance();
	~GameInstance();
};

