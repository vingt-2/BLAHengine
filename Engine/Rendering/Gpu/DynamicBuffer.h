// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "Resource.h"
#include "ResourceTypes.h"

namespace BLA
{
    namespace Gpu
    {
        struct BaseDynamicBuffer : BaseResource
        {
            friend class Interface;

            static const EResourceType ms_resourceType = EResourceType::eDynamicBuffer;

            const void* GetData() const;
            void* GetData();

            blaU32 GetSize() const;

            //TODO Set by current backend
            static blaU32 ms_allocationHandleSize;
            union AllocationHandle_t
            {
                blaU64 bits64;
                void* pointer;
            } m_allocationHandle;

        protected:
            BaseDynamicBuffer(blaSize elementSize);

            blaU32 m_size;
            void* m_dataPointer;
        };

        template<typename T>
        class DynamicBuffer : public BaseDynamicBuffer
        {
        public:
            DynamicBuffer() : BaseDynamicBuffer(sizeof(T)) {}

            T* GetData() { return static_cast<T*>(m_dataPointer); }
            const T* GetData() const { return static_cast<const T*>(m_dataPointer); }
        };
    }
};