// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StaticBuffer.h"
#include "Interface.h"

// Handle alignment concerns when required
namespace BLA::Gpu
{
    const void* BaseStaticBuffer::GetData() const
    {
        return m_dataPointer;
    }

    void* BaseStaticBuffer::GetData()
    {
        return m_dataPointer;
    }

    blaU32 BaseStaticBuffer::GetLength() const
    {
        return m_bufferLength;
    }

    blaU32 BaseStaticBuffer::GetElementSize() const
    {
        return m_elementSize;
    }

    BaseStaticBuffer::BaseStaticBuffer(blaU32 length, blaSize elementSize, Usage usage) : 
        m_usage(usage), BaseResource(EResourceType::eStaticBuffer), 
        m_bufferLength(length), 
        m_elementSize(static_cast<blaU32>(elementSize))
    {
        memset(&m_StagingData, 0, sizeof(StagingData_t));
        memset(&m_allocationHandle, 0, sizeof(AllocationHandle_t));
        Interface* gpuInterface = Interface::GetSingletonInstance();

        BLA_ASSERT(gpuInterface);

        gpuInterface->PrepareForStaging(this);
    }

}
