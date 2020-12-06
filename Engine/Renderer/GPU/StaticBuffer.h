// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "ResourceTypes.h"

namespace BLA
{
    //TODO: This honestly should directly be a staging buffer, 
    //TODO: this way resources are first memcpied into a staging buffer ready for transfer
    namespace Gpu
    {
        struct BaseStaticBuffer
        {
            static const EResourceType ms_resourceType = EResourceType::eStaticBuffer;

            const void* GetData() const;
            void* GetData();
            blaU32 GetLength() const;
            blaU32 GetElementSize() const;

            //TODO Set by current backend
            static blaU32 ms_allocationHandleSize;
            union AllocationHandle_t
            {
                blaU64 bits64;
                void* pointer;
            } m_allocationHandle;

        protected:
            BaseStaticBuffer(blaU32 size);
            blaU32 m_bufferLength;
            blaU32 m_elementSize;
            static BaseStaticBuffer* New(blaSize length, blaSize elementSize);
            static void Delete(BaseStaticBuffer* baseBuffer);
        };

        template<typename T>
        class StaticBuffer : public BaseStaticBuffer
        {
            StaticBuffer(blaSize size) : BaseStaticBuffer(static_cast<blaU32>(size)) {}

        public:
            ~StaticBuffer() = delete;
            static StaticBuffer<T>* New(blaSize size)
            {
                void* p = BaseStaticBuffer::New(size, sizeof(T));

                return new (p) StaticBuffer<T>(size);
            }
        };
    }
};