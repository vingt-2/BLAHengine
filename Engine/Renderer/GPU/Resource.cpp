// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Resource.h"
#include "Interface.h"

namespace BLA::Gpu
{
    void BaseResource::Submit()
    {
        Interface* gpuInterface = Interface::GetSingletonInstance();
        BLA_ASSERT(gpuInterface);

        m_handle = gpuInterface->Submit(this);
    }

    void BaseResource::Cancel()
    {
        Interface* gpuInterface = Interface::GetSingletonInstance();
        BLA_ASSERT(gpuInterface);

        gpuInterface->Cancel(m_handle);
        m_handle = ResourceHandle();
    }

}
