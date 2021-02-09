// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Interface.h"
#include "RenderPassProgram.h"

// TODO: This public interface isn't very necessary, probably best to be hidden away in the vulkan impl abstract
namespace BLA
{
    namespace System::Vulkan
    {
        class Context;
    }

    namespace Gpu
    {        
        struct BaseStaticBuffer;
        struct BaseImage;
        class RenderPassInstanceImplementation;

        class Vulkan : public Interface
        {
            struct VulkanImplementation;
        public:
            Vulkan(const System::Vulkan::Context* context);

            ResourceHandle Submit(BaseResource* resource) override;
            void Cancel(BaseResource* resource) override;
            void PrepareForStaging(BaseResource* resource) override;
            ResourceHandle PrepareDynamicBuffer(BaseResource* resource) override;
            
        protected:
            RenderPassInstanceImplementation* SetupRenderPass(const RenderPassDescriptor* rpDescriptor, RenderPassProgram& program) override;
            void AttachToRenderPass(RenderPassInstanceImplementation* rpInstanceImplementation, const BaseRenderPassAttachment* attachment) override;

            void Render(RenderPassInstanceImplementation* renderPassInstanceImplementation) override;
            void RegisterRenderPassObjectBase(RenderPassInstanceImplementation* renderPassInstanceImplementation, const BaseRenderPassObject& instance) override;
        
        private:
            ResourceHandle SubmitStaticBuffer(BaseStaticBuffer* resource);
            ResourceHandle SubmitImage(BaseImage* resource);
            ResourceHandle SubmitShaderProgram(ShaderProgram* shaderProgram);

            void CancelStaticBuffer(BaseStaticBuffer* resource);
            void CancelImage(BaseImage* resource);
            void CancelShaderProgram(ShaderProgram* shaderProgram);

            VulkanImplementation* m_implementation;

        public:
            // Vulkan specific public stuff that I call externallyt as a temporary wiring

            
        };
    }
};