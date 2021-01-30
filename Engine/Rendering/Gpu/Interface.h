// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "BLASingleton.h"
#include "System.h"
#include "Resource.h"

namespace BLA
{
    class Renderer;
    class BaseRenderPassObject;
    
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
            BLA_DECLARE_THREAD_LOCAL_SINGLETON(Interface)
        public:
            virtual ResourceHandle Submit(BaseResource* resource) = 0;
            virtual void Cancel(BaseResource* handle) = 0;
            virtual void PrepareForStaging(BaseResource* resource) = 0;

            virtual ResourceHandle PrepareDynamicBuffer(BaseResource* resource) = 0;

            template<class RenderPass>
            void RegisterRenderPassObject(const typename RenderPass::RenderPassObject& instance)
            {
                RegisterRenderPassObjectBase(*RenderPass::GetSingletonInstance()->m_pRenderPassDescriptor, instance);
            }
        
        protected:
            static void SetBufferDataPointer(BaseStaticBuffer* buffer, blaU8* pointer);
            static void SetBufferDataPointer(BaseDynamicBuffer* buffer, blaU8* pointer);
            static BaseStaticBuffer* GetImageBuffer(BaseImage* image);

            virtual void SetupRenderPass(RenderPassDescriptor& renderPassDescriptor, RenderPassProgram& program) = 0;
            // virtual void AttachToRenderPass(RenderPassDescriptor& renderPassDescriptor, BaseRenderPassAttachment& attachment) = 0;
            
            virtual void Render(RenderPassDescriptor& renderPassDescriptor) = 0;
            virtual void RegisterRenderPassObjectBase(const RenderPassDescriptor& descriptor, const BaseRenderPassObject& instance) = 0;
        };
    }
};