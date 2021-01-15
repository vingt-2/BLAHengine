// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "StdInclude.h"
#include "BLAStringID.h"
#include "Resource.h"

namespace BLA
{
    namespace Core::InspectableVariables
    {
        struct ExposedVarTypeDescriptor;
    }

    namespace Gpu
    {
        struct ShaderProgram : BaseResource 
        {
            enum class Type { VertexShader, FragmentShader } m_type;
            
            ShaderProgram(Type type, blaString pathToBinaries /*TODO: Replace with a loaded blaengine asset ...*/) : BaseResource(EResourceType::eShaderProgram), m_type(type), m_pathToBinaries(pathToBinaries) {}
        
            blaString m_pathToBinaries; //TODO: Replace with a loaded blaengine asset ...
        };

        struct RenderPassProgram
        {
            blaVector<ShaderProgram> m_shaders;
        };
    }
};