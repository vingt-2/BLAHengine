// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"

namespace BLA
{
    namespace Gpu
    {
        enum class EResourceType : blaU8
        {
            eStaticBuffer = 0,
            eDynamicBuffer,
            eImage,
            eEnd
        };
    }
};