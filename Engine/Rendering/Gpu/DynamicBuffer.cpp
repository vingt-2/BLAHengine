// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "DynamicBuffer.h"

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

    blaU32 BaseDynamicBuffer::GetLength() const
    {
        return m_bufferLength;
    }

    blaU32 BaseDynamicBuffer::GetElementSize() const
    {
        return m_elementSize;
    }

    BaseDynamicBuffer::BaseDynamicBuffer(blaU32 length, blaSize elementSize) : m_bufferLength(length), m_elementSize(static_cast<blaU32>(elementSize))
    {
    }
}
