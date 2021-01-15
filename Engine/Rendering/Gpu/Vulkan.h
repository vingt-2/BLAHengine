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
        struct Image;
        class RenderPassImplementation;

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
            void SetupRenderPass(RenderPassDescriptor& renderPassDescriptor, RenderPassProgram& program) override;
            void AttachToRenderPass(RenderPassDescriptor& renderPassDescriptor, RenderAttachment& attachment) override;

            void Render(RenderPassDescriptor& renderPassDescriptor) override;
            void RegisterRenderPassInstanceBase(const RenderPassDescriptor& descriptor, const BaseRenderPassInstance& instance) override;
        
        private:
            ResourceHandle SubmitStaticBuffer(BaseStaticBuffer* resource);
            void CancelStaticBuffer(ResourceHandle handle);

            ResourceHandle SubmitImage(Image* resource);

            ResourceHandle SubmitShaderProgram(ShaderProgram* shaderProgram);

            VulkanImplementation* m_implementation;

        public:
            // Vulkan specific public stuff that I call externallyt as a temporary wiring

            
        };
    }
};