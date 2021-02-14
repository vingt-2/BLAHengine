// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "BLASingleton.h"
#include "System.h"
#include "Resource.h"
#include "Rendering/RenderPass.h"

namespace BLA
{
    class Renderer;
    class BaseRenderPassObject;
    struct BaseRenderPassInstance;

    namespace Gpu
    {
        struct BaseImage;
        struct BaseStaticBuffer;
        struct BaseDynamicBuffer;
        struct RenderPassDescriptor;
        struct RenderPassProgram;
        struct BaseRenderPassAttachment;

        class RenderPassInstanceImplementation;
        
        class Interface
        {
            friend class Renderer;
            friend struct BaseRenderPassInstance;

            BLA_DECLARE_THREAD_LOCAL_SINGLETON(Interface)

        public:
            virtual ResourceHandle Submit(BaseResource* resource) = 0;
            virtual void Cancel(BaseResource* handle) = 0;
            virtual void PrepareForStaging(BaseResource* resource) = 0;

            virtual ResourceHandle PrepareDynamicBuffer(BaseResource* resource) = 0;
        
        protected:
            static void SetBufferDataPointer(BaseStaticBuffer* buffer, blaU8* pointer);
            static void SetBufferDataPointer(BaseDynamicBuffer* buffer, blaU8* pointer);
            static BaseStaticBuffer* GetImageBuffer(BaseImage* image);

            virtual RenderPassInstanceImplementation* SetupRenderPass(const RenderPassDescriptor* rpDescriptor, RenderPassProgram& program) = 0;
            virtual void AttachToRenderPass(RenderPassInstanceImplementation* rpInstanceImplementation, const BaseRenderPassAttachment* attachment) = 0;
            
            virtual void Render(RenderPassInstanceImplementation* renderPassInstanceImplementation, BaseRenderPassInstance::RenderPassObjectIterator& iterator) = 0;
            virtual RenderPassObjectHandle RegisterRenderPassObjectBase(RenderPassInstanceImplementation* renderPassInstanceImplementation, const BaseRenderPassObject& instance) = 0;
        };
    }
};