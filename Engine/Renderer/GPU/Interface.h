// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "BLASingleton.h"
#include "System.h"
#include "Resource.h"

namespace BLA
{
    class Renderer;
    namespace Gpu
    {
        struct Image;
        struct BaseStaticBuffer;
        struct RenderPassDescriptor;
        class RenderPassImplementation;

        class Interface
        {
            friend class Renderer;
            BLA_DECLARE_THREAD_LOCAL_SINGLETON(Interface)
        public:
            virtual ResourceHandle Submit(BaseResource* resource) = 0;
            virtual void Cancel(ResourceHandle handle) = 0;
            virtual void PrepareForStaging(BaseResource* resource) = 0;

        protected:
            static void SetBufferDataPointer(BaseStaticBuffer* buffer, blaU8* pointer);
            static BaseStaticBuffer* GetImageBuffer(Image* image);

            virtual RenderPassImplementation* SetupRenderPass(RenderPassDescriptor& renderPassDescriptor) = 0;
        };
    }
};