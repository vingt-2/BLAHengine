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

    Image::Image(blaIVec2 size, BaseStaticBuffer* buffer) : BaseResource(EResourceType::eImage), m_size(size), m_buffer(buffer), m_elementSize(buffer->GetElementSize())
    {
        BLA_TRAP(buffer);
        BLA_TRAP(size.x * size.y == buffer->GetLength());
    }
}
