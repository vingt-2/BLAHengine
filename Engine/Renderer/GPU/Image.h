// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "Maths/Maths.h"
#include "ResourceTypes.h"
#include "Resource.h"
#include "StaticBuffer.h"

namespace BLA
{
    namespace Gpu
    {
        struct BaseStaticBuffer;

        struct Image : BaseResource
        {
            friend class Interface;

            static const EResourceType ms_resourceType = EResourceType::eImage;

            const void* GetData() const;
            void* GetData();
            blaIVec2 GetSize() const;
            blaU32 GetElementSize() const;

            //TODO Set by current backend
            static blaU32 ms_allocationHandleSize;
            union AllocationHandle_t
            {
                blaU64 bits64;
                void* pointer;
            } m_allocationHandle;

            Image(blaIVec2 size, BaseStaticBuffer* buffer);
        protected:
            blaIVec2 m_size;
            blaU32 m_elementSize;
            BaseStaticBuffer* m_buffer;
        };
    }
};