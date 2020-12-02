// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "BLASingleton.h"
#include "System.h"
#include "Resource.h"

namespace BLA
{
    namespace GPU
    {
        class Interface
        {
            BLA_DECLARE_THREAD_SINGLETON(Interface)
        public:
            virtual ResourceHandle Submit(BaseResource* resource) = 0;
            virtual void Cancel(ResourceHandle handle) = 0;
        };
    }
};