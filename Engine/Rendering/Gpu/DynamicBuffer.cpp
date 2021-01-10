// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "DynamicBuffer.h"
#include "Interface.h"

// Handle alignment concerns when required

namespace BLA::Gpu
{
    const void* BaseDynamicBuffer::GetData() const
    {
        return m_dataPointer;
    }

    void* BaseDynamicBuffer::GetData()
    {
        return m_dataPointer;
    }
	
    blaU32 BaseDynamicBuffer::GetSize() const
    {
        return m_size;
    }

    BaseDynamicBuffer::BaseDynamicBuffer(blaSize elementSize) :
	BaseResource(EResourceType::eDynamicBuffer),
	m_size(static_cast<blaU32>(elementSize))
    {
        memset(&m_allocationHandle, 0, sizeof(AllocationHandle_t));
        Interface* gpuInterface = Interface::GetSingletonInstance();

        BLA_ASSERT(gpuInterface);

        m_handle = gpuInterface->PrepareDynamicBuffer(this);
    }
}
