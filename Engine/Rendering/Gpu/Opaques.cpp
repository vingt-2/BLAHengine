// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Opaques.h"
#include "Core/InspectableVariables.h"
#include "Interface.h"

namespace BLA
{
    // Make sure the Inspectable definitions are exported !
    namespace Gpu
    {
        struct SamplerDescriptor : Core::InspectableVariables::ExposedVarTypeDescriptor
        {
            SamplerDescriptor() : ExposedVarTypeDescriptor{ BlaStringId("Sampler"), sizeof(Sampler) } {}
        };
    }

    namespace Core::InspectableVariables
    {

        template<>
        BLACORE_API ExposedVarTypeDescriptor* GetPrimitiveDescriptor<Gpu::Sampler>()
        {
            static Gpu::SamplerDescriptor samplerDescriptor;
            return &samplerDescriptor;
        }

    }
}
