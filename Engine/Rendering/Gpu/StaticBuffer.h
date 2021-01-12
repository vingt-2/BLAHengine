// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "ResourceTypes.h"
#include "Resource.h"

namespace BLA
{
    //TODO: This honestly should directly be a staging buffer, 
    //TODO: this way resources are first memcpied into a staging buffer ready for transfer
    namespace Gpu
    {
        struct BaseStaticBuffer : BaseResource
        {
            friend class Interface;

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
            BaseStaticBuffer(blaU32 size, blaSize elementSize);
            blaU32 m_bufferLength;
            blaU32 m_elementSize;
            void* m_dataPointer;
        };

        template<typename T>
        class StaticBuffer : public BaseStaticBuffer
        {
        public:
            StaticBuffer(blaU32 length) : BaseStaticBuffer(length, sizeof(T)) {}

        	T& operator[](int i)
        	{
                return *(static_cast<T*>(GetData()) + i);
        	}
        };
    };
};