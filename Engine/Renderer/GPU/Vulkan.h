// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Interface.h"

namespace BLA
{
    namespace GPU
    {
        struct BaseBuffer;
        class Vulkan : public Interface
        {
            struct VulkanImplementation;
        public:
            virtual ResourceHandle Submit(BaseResource* resource);
            virtual void Cancel(ResourceHandle handle);

        private:
            ResourceHandle SubmitStaticBuffer(StaticResource<BaseBuffer>* resource);
            void CancelStaticBuffer(ResourceHandle handle);

            VulkanImplementation* m_implementation;
        };
    }
};