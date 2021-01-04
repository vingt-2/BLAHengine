// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "StdInclude.h"
#include "BLAStringID.h"
namespace BLA
{
    namespace Core::InspectableVariables
    {
        struct ExposedVarTypeDescriptor;
    }

    namespace Gpu
    {
        class RenderPassImplementation
        {

        };

        struct RenderPassDescriptor
        {
            blaStringId m_name;
            blaU32 m_attachmentCount;
            RenderPassImplementation* m_pToInstanceRenderPassDescriptorPointer;
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_vertexAttributesDescriptors;
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_uniformValuesDescriptors;
        };
    }
};