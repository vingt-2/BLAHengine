// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "ShadowRender.h"

namespace BLA
{
    class BLACORE_API DirectionalLightRender
    {
    public:
        GLuint m_lightRenderPrgmID;

        DirectionalShadowRender m_shadowRender;
    };
}