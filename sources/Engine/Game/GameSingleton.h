#pragma once
#include "../Renderer/Renderer.h"
#include "../AssetsManager/SharedResources.h"

class GameSingleton
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