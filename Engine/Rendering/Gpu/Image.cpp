// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Image.h"

// Handle alignment concerns when required

namespace BLA::Gpu
{
    blaIVec2 BaseImage::GetSize() const
    {
        return m_size;
    }

    blaU32 BaseImage::GetElementSize() const
    {
        return m_elementSize;
    }

    BaseImage::BaseImage(blaIVec2 size, BaseStaticBuffer* buffer) : BaseResource(EResourceType::eImage), m_size(size), m_buffer(buffer), m_elementSize(buffer->GetElementSize())
    {
        BLA_TRAP(buffer);
        BLA_TRAP(size.x * size.y == buffer->GetLength());
    }
}
