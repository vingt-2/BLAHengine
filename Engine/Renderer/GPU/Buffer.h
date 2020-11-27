// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "ResourceTypes.h"

namespace BLA
{
    namespace GPU
    {
        struct BaseBuffer
        {
            static const EResourceType ms_resourceType = EResourceType::eStaticBuffer;

            const void* GetData() const;
            void* GetData();
            blaSize GetLength() const;
        protected:
            BaseBuffer(blaSize size);
            blaSize m_bufferLength;
            static BaseBuffer* New(blaSize length, blaSize elementSize);
            static void Delete(BaseBuffer* baseBuffer);
        };

        template<typename T>
        class Buffer : public BaseBuffer
        {
            Buffer(blaSize size) : BaseBuffer(size) {}

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