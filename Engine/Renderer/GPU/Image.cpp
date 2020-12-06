// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Image.h"

// Handle alignment concerns when required

namespace BLA::Gpu
{
    const void* Image::GetData() const
    {
        return static_cast<const void*>(this + 1);
    }

    void* Image::GetData()
    {
        return static_cast<void*>(this + 1);
    }

    blaIVec2 Image::GetSize() const
    {
        return m_size;
    }

    blaU32 Image::GetElementSize() const
    {
        return m_elementSize;
    }

    Image::Image(blaIVec2 size, blaSize elementSize) : m_size(size), m_elementSize(static_cast<blaU32>(elementSize))
    {}

    Image* Image::New(blaIVec2 size, blaSize elementSize)
    {
        blaU8* storage = (new blaU8[sizeof(Image) + size.x * size.y * elementSize]);

        return new (storage) Image(size, elementSize);
    }

    void Image::Delete(Image* baseBuffer)
    {
        delete baseBuffer;
    }
}
