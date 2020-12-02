// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "ResourceTypes.h"

namespace BLA
{
    namespace GPU
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
            BaseResource() : m_handle{0}
            {}

            bool IsReady() const { return m_handle.m_impl.bits64 != 0; }

        protected:
            ResourceHandle m_handle;
        };

        template<typename T>
        struct StaticResource : BaseResource
        {
            // TODO Statically check type validity (can be a primitive type, a gpu buffer, a texture ...)
            friend class Renderer;

            StaticResource(T& resource) : BaseResource(), m_resource(resource)
            {}

            ~StaticResource<T>()
            {
                Cancel();
            }

            T& m_resource;

        private:

            void Submit()
            {
                extern ResourceHandle(*g_GPUImplementationResourceSubmitFunctionTable[static_cast<blaSize>(EResourceType::eEnd)])(void*);
                m_handle = reinterpret_cast<ResourceHandle>(g_GPUImplementationResourceSubmitFunctionTable[T::ms_resourceType](&m_resource));
            }

            void Cancel()
            {
                extern void* (*g_GPUImplementationResourceDeleteFunctionTable[static_cast<blaSize>(EResourceType::eEnd)])(ResourceHandle);
                g_GPUImplementationResourceDeleteFunctionTable[T::ms_resourceType](m_handle);
            }
        };
    }
};