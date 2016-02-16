#pragma once
#include "RenderingManager.h"
#include "../Renderer/Renderer.h"
class GameInstance
{
public:

	RenderingManager* m_renderingManager;

	Renderer* m_renderer;

	GameInstance();
	~GameInstance();
};

