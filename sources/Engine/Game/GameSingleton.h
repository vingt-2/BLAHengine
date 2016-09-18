#pragma once
#include "../Renderer/Renderer.h"
#include "../Assets/AssetsManager.h"
#include "..\..\Common\StdInclude.h"

class BLACORE_API GameSingleton
{
public:
    Renderer* renderer;
    SharedResources* resources;

    GameSingleton(Renderer* renderer,SharedResources* resources)
    {
        this->renderer = renderer;
        this->resources = resources;
    }
};