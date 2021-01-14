// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "BLASingleton.h"
#include "System.h"
#include "Resource.h"

namespace BLA
{
    class Renderer;
    class BaseRenderPassInstance;
	
    namespace Gpu
    {
        struct Image;
        struct BaseStaticBuffer;
        struct BaseDynamicBuffer;
        struct RenderPassDescriptor;
        struct RenderPassProgram;
        struct RenderAttachment;
    	
        class RenderPassImplementation;
    	
        class Interface
        {
            friend class Renderer;
            BLA_DECLARE_THREAD_LOCAL_SINGLETON(Interface)
        public:
            virtual ResourceHandle Submit(BaseResource* resource) = 0;
            virtual void Cancel(ResourceHandle handle) = 0;
            virtual void PrepareForStaging(BaseResource* resource) = 0;

            virtual ResourceHandle PrepareDynamicBuffer(BaseResource* resource) = 0;

            template<class RenderPass>
            void RegisterRenderPassInstance(const typename RenderPass::RenderPassInstance& instance)
            {
                RegisterRenderPassInstanceBase(*RenderPass::GetSingletonInstance()->m_pRenderPassDescriptor, instance);
            }
        
        protected:
            static void SetBufferDataPointer(BaseStaticBuffer* buffer, blaU8* pointer);
            static void SetBufferDataPointer(BaseDynamicBuffer* buffer, blaU8* pointer);
            static BaseStaticBuffer* GetImageBuffer(Image* image);

            virtual void SetupRenderPass(RenderPassDescriptor& renderPassDescriptor, RenderPassProgram& program) = 0;
            virtual void AttachToRenderPass(RenderPassDescriptor& renderPassDescriptor, RenderAttachment& attachment) = 0;
        	
            virtual void Render(RenderPassDescriptor& renderPassDescriptor) = 0;
            virtual void RegisterRenderPassInstanceBase(const RenderPassDescriptor& descriptor, const BaseRenderPassInstance& instance) = 0;
        };
    }
};