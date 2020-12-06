// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StaticBuffer.h"

// Handle alignment concerns when required

namespace BLA::Gpu
{
    const void* BaseStaticBuffer::GetData() const
    {
        return static_cast<const void*>(this + 1);
    }

    void* BaseStaticBuffer::GetData()
    {
        return static_cast<void*>(this + 1);
    }

    blaU32 BaseStaticBuffer::GetLength() const
    {
        return m_bufferLength;
    }

    blaU32 BaseStaticBuffer::GetElementSize() const
    {
        return m_elementSize;
    }

    BaseStaticBuffer::BaseStaticBuffer(blaU32 size)
    {
        m_bufferLength = size;
    }

    BaseStaticBuffer* BaseStaticBuffer::New(blaSize length, blaSize elementSize)
    {
        return reinterpret_cast<BaseStaticBuffer*>(new blaU8[sizeof(BaseStaticBuffer) + length * elementSize]);
    }

    void BaseStaticBuffer::Delete(BaseStaticBuffer* baseBuffer)
    {
        delete baseBuffer;
    }
}
