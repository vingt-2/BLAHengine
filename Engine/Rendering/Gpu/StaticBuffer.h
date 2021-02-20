// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "ResourceTypes.h"
#include "Resource.h"
#include "StdInclude.h"

namespace BLA
{
    //TODO: This honestly should directly be a staging buffer, 
    //TODO: this way resources are first memcpied into a staging buffer ready for transfer
    namespace Gpu
    {
        struct BaseStaticBuffer : BaseResource
        {
            friend class Interface;

            enum class Usage
            {
                VertexBuffer,
                IndexBuffer,
                ImageBuffer
            } m_usage;
            
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

            union StagingData_t
            {
                blaU64 bits64[2];
                void* pointers[2];
            } m_StagingData;

        protected:
            BaseStaticBuffer(blaU32 size, blaSize elementSize, Usage usage);
            blaU32 m_bufferLength;
            blaU32 m_elementSize;
            void* m_dataPointer;
        };

        template<typename T>
        class StaticBuffer : public BaseStaticBuffer
        {
        public:
            //TODO: Crashes when length is 0
            StaticBuffer(blaU32 length, Usage usage) : BaseStaticBuffer(length, sizeof(T), usage) {}

            T& operator[](int i)
            {
                return *(static_cast<T*>(GetData()) + i);
            }

            void CopyFromVector(const blaVector<T>& dataVector)
            {
                BLA_ASSERT(dataVector.size() == GetLength());
                memcpy_s(m_dataPointer, sizeof(T) * m_bufferLength, dataVector.data(), dataVector.size() * sizeof(T));
            }
        };
    };
};