// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "StdInclude.h"
#include "BLAStringID.h"
#include "Formats.h"

namespace BLA
{
    namespace Core::InspectableVariables
    {
        struct ExposedVarTypeDescriptor;
    }

    namespace Gpu
    {
        struct RPAttachmentDescription
        {
            blaVector<Gpu::Formats::Enum::Index> m_colorAttachments;

            Gpu::Formats::Enum::Index m_depthAttachment;
        };

        class RenderPassInstanceImplementation
        {

        };

        struct RenderPassDescriptor
        {
            blaStringId m_name;
            RPAttachmentDescription m_attachmentDescription;
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_vertexAttributesDescriptors;
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_uniformValuesDescriptors;
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_opaqueValuesDescriptors;
        };
    }
};