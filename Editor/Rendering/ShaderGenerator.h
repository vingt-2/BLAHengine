// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"

namespace BLA
{
    namespace Gpu
    {
        struct RenderPassDescriptor;
    }

    struct ShaderCode
    {
        blaString m_code;
    };
	
    struct ShaderGenerator
    {
        void GenerateVertexShader(ShaderCode& code);

        const Gpu::RenderPassDescriptor* m_pDescriptor;
    };
}
