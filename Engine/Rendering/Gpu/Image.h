// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "Formats.h"
#include "ResourceTypes.h"
#include "Resource.h"
#include "StaticBuffer.h"

namespace BLA
{
    namespace Gpu
    {
        struct BaseStaticBuffer;

        struct BaseImage : BaseResource
        {
            friend class Interface;

            static const EResourceType ms_resourceType = EResourceType::eImage;

            blaIVec2 GetSize() const;
            blaU32 GetElementSize() const;

            //TODO Set by current backend
            static blaU32 ms_allocationHandleSize;
            union AllocationHandle_t
            {
                blaU64 bits64;
                void* pointer;
            } m_allocationHandle;

            Formats::Enum::Index GetFormat() const { return m_format; }

        protected:
            BaseImage(blaIVec2 size, Formats::Enum::Index format, BaseStaticBuffer* buffer);

            blaIVec2 m_size;
            blaU32 m_elementSize;
            Formats::Enum::Index m_format;
            BaseStaticBuffer* m_buffer;
        };

        template<typename Format>
        struct Image : BaseImage
        {
            Image(blaIVec2 size, StaticBuffer<typename Format::AssociatedType>& buffer) : BaseImage(size, Format::ms_formatIndex, &buffer) {}

            StaticBuffer<typename Format::AssociatedType>& GetBuffer() { return static_cast<StaticBuffer<typename Format::AssociatedType>>(*m_buffer); }
        };
    }
};