// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved
#pragma once

#include "Core/InspectableVariables.h"
#include "Gpu/Resource.h"
#include "Renderer/RenderPass.h"

namespace BLA
{
    class RenderWindow;
    class Renderer
    {
    public:
        Renderer() {};

        virtual bool Update() = 0;
        //TODO: Remove, improve DevGuiwindow design ...
        virtual void SetViewportSize(blaIVec2 viewportSize) = 0;
        virtual RenderWindow* GetRenderWindow() = 0;
        virtual ~Renderer(void) = default;

    protected:
        blaIVec2 m_viewPortExtents;
    };
}