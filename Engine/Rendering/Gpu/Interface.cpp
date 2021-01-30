// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Interface.h"
#include "StaticBuffer.h"
#include "DynamicBuffer.h"
#include "Image.h"
#include "Formats.h"

namespace BLA::Gpu
{
    BLA_IMPLEMENT_THREAD_LOCAL_SINGLETON(Interface)

    void Interface::SetBufferDataPointer(BaseStaticBuffer* buffer, blaU8* pointer)
    {
        buffer->m_dataPointer = pointer;
    }

    void Interface::SetBufferDataPointer(BaseDynamicBuffer* buffer, blaU8* pointer)
    {
        buffer->m_dataPointer = pointer;
    }

    BaseStaticBuffer* Interface::GetImageBuffer(BaseImage* image)
    {
        return image->m_buffer;
    }

    blaMap<blaStringId, Formats::Enum::Index> Formats::g_fundamentalTypeToFormat =
    {
        { BlaStringId("blaVec3"), Formats::Enum::Index::R32G32B32_SFLOAT },
        { BlaStringId("blaVec2"), Formats::Enum::Index::R32G32_SFLOAT },
        { BlaStringId("blaU32"), Formats::Enum::Index::R8G8B8A8_UINT }
    };
}
