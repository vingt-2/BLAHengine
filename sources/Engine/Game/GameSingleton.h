#pragma once
#include "../Renderer/Renderer.h"
#include "../Assets/AssetsManager.h"
#include "..\..\Common\StdInclude.h"

namespace BLAengine
{
	class BLACORE_API GameSingleton
	{
	public:
		Renderer* renderer;
		AssetManager* resources;

		GameSingleton(Renderer* renderer, AssetManager* resources)
		{
			this->renderer = renderer;
			this->resources = resources;
		}
	};
}
