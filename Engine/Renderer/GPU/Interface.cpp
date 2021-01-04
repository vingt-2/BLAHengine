// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Interface.h"
#include "StaticBuffer.h"
#include "Image.h"

namespace BLA::Gpu
{
    BLA_IMPLEMENT_THREAD_LOCAL_SINGLETON(Interface)

    void Interface::SetBufferDataPointer(BaseStaticBuffer* buffer, blaU8* pointer)
    {
        buffer->m_dataPointer = pointer;
    }

    BaseStaticBuffer* Interface::GetImageBuffer(Image* image)
    {
        return image->m_buffer;
    }
}
