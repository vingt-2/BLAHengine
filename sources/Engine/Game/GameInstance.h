#pragma once
#include "RenderingManager.h"
#include "../Renderer/Renderer.h"
#include "..\..\Common\StdInclude.h"

class BLACORE_API GameInstance
{
public:

	RenderingManager* m_renderingManager;

	Renderer* m_renderer;

	GameInstance();
	~GameInstance();
};

