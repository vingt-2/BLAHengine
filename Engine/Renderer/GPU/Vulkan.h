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
        
        class Vulkan : public Interface
        {
            struct VulkanImplementation;
        public:
            Vulkan(const System::Vulkan::Context* context);

            virtual ResourceHandle Submit(BaseResource* resource);
            virtual void Cancel(ResourceHandle handle);

        private:
            ResourceHandle SubmitStaticBuffer(Resource<BaseStaticBuffer>& resource);
            void CancelStaticBuffer(ResourceHandle handle);

            ResourceHandle SubmitImage(Resource<Image>& resource);

            VulkanImplementation* m_implementation;
        };
    }
};