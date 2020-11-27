// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Buffer.h"

// Handle alignment concerns when required

namespace BLA::GPU
{
    const void* BaseBuffer::GetData() const
    {
        return static_cast<const void*>(this + 1);
    }

    void* BaseBuffer::GetData()
    {
        return static_cast<void*>(this + 1);
    }

    blaSize BaseBuffer::GetLength() const
    {
        return m_bufferLength;
    }

    BaseBuffer::BaseBuffer(blaSize size)
    {
        m_bufferLength = size;
    }

    BaseBuffer* BaseBuffer::New(blaSize length, blaSize elementSize)
    {
        return reinterpret_cast<BaseBuffer*>(new blaU8[sizeof(BaseBuffer) + length * elementSize]);
    }

    void BaseBuffer::Delete(BaseBuffer* baseBuffer)
    {
        delete baseBuffer;
    }
}
