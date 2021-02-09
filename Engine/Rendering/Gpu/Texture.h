// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "ResourceTypes.h"
#include "Resource.h"

namespace BLA
{
    namespace Gpu
    {
        struct BaseImage;

        struct BaseTexture : BaseResource
        {
            friend class Interface;

            static const EResourceType ms_resourceType = EResourceType::eTexture;

            const BaseImage* GetImage() { return m_image; }
            
        protected:
            BaseTexture(BaseImage* image): BaseResource(ms_resourceType), m_image(image) {}

            BaseImage* m_image;
        };

        template<typename Format>
        struct Texture : BaseTexture
        {
            Texture(Image<Format>* image) : BaseTexture(image) {}
        };
    }
};