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
        class RPAttachmentDescription
        {

        };

        class RenderPassInstanceImplementation
        {

        };

        struct RenderPassDescriptor
        {
            blaStringId m_name;
            RPAttachmentDescription m_attachmentCount;
            RenderPassInstanceImplementation* m_pToInstanceRenderPassDescriptorPointer;
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_vertexAttributesDescriptors;
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_uniformValuesDescriptors;
        };
    }
};