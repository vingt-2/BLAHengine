// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Resource.h"
#include "Interface.h"

namespace BLA::Gpu
{
    BaseResource::~BaseResource()
    {
        Interface* gpuInterface = Interface::GetSingletonInstance();
        BLA_ASSERT(gpuInterface);

        gpuInterface->Cancel(this);
    }

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

        gpuInterface->Cancel(this);
        m_handle = ResourceHandle();
    }

}
