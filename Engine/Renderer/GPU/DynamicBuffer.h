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
        struct BaseDynamicBuffer
        {
            static const EResourceType ms_resourceType = EResourceType::eDynamicBuffer;

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
            BaseDynamicBuffer(blaU32 length, blaSize elementSize);
            blaU32 m_bufferLength;
            blaU32 m_elementSize;
            void* m_dataPointer;
        };

        template<typename T>
        class DynamicBuffer : public BaseDynamicBuffer
        {
            DynamicBuffer(blaU32 length) : BaseDynamicBuffer(length, sizeof(T)) {}

        public:
            ~DynamicBuffer() = delete;
        };
    }
};