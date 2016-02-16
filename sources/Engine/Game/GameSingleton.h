#pragma once
#include "../Renderer/Renderer.h"
#include "../AssetsManager/SharedResources.h"

class GameSingleton
{
public:
    const Renderer* renderer;
    const SharedResources* resources;

    GameSingleton(const Renderer* renderer,const SharedResources* resources)
    {
        this->renderer = renderer;
        this->resources = resources;
    }
};