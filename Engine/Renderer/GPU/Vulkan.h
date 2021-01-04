// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Interface.h"

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
            void Cancel(ResourceHandle handle) override;
            void PrepareForStaging(BaseResource* resource) override;
        protected:
            RenderPassImplementation* SetupRenderPass(RenderPassDescriptor& renderPassDescriptor) override;
        private:
            ResourceHandle SubmitStaticBuffer(BaseStaticBuffer* resource);
            void CancelStaticBuffer(ResourceHandle handle);

            ResourceHandle SubmitImage(Image* resource);

            VulkanImplementation* m_implementation;


        };
    }
};