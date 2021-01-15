// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "ResourceTypes.h"

namespace BLA
{
    namespace Gpu
    {
        struct ResourceHandle
        {
            //TODO Set by current backend
            static blaU32 ms_implSize;
            union
            {
                blaU64 bits64;
                void* pointer;
            } m_impl;
        };

        struct BaseResource
        {
            BaseResource(EResourceType type) : m_handle{0}, m_resourceType(type) {}
            virtual ~BaseResource();

            bool IsReady() const { return m_handle.m_impl.bits64 != 0; }
            EResourceType GetType() const { return m_resourceType; }

            void Submit();
            void Cancel();

            ResourceHandle GetHandle() const { return m_handle; }

        protected:
            ResourceHandle m_handle;
            EResourceType m_resourceType;
        };
    }
};