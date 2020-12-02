// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "ResourceTypes.h"

namespace BLA
{
    //TODO: This honestly should directly be a staging buffer, 
    //TODO: this way resources are first memcpied into a staging buffer ready for transfer
    namespace GPU
    {
        struct BaseBuffer
        {
            static const EResourceType ms_resourceType = EResourceType::eStaticBuffer;

            const void* GetData() const;
            void* GetData();
            blaU32 GetLength() const;
            blaU32 GetElementSize() const;
        protected:
            BaseBuffer(blaU32 size);
            blaU32 m_bufferLength;
            blaU32 m_elementSize;
            static BaseBuffer* New(blaSize length, blaSize elementSize);
            static void Delete(BaseBuffer* baseBuffer);
        };

        template<typename T>
        class Buffer : public BaseBuffer
        {
            Buffer(blaSize size) : BaseBuffer(static_cast<blaU32>(size)) {}

        public:
            ~Buffer() = delete;
            static Buffer<T>* New(blaSize size)
            {
                void* p = BaseBuffer::New(size, sizeof(T));

                return new (p) Buffer<T>(size);
            }
        };
    }
};