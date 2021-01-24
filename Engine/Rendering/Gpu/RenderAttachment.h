// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

namespace BLA
{
    namespace Gpu
    {
        struct BaseImage;
        //template<typename attachmentDataType> // <- The data type describing each entry in the attachment buffer // Also determines sample count for vk ?
        struct RenderAttachment
        {
            RenderAttachment(Gpu::BaseImage* image) : m_image(image), m_sampleCount(1) {}
            
            Gpu::BaseImage* m_image;
            blaU8 m_sampleCount; // (per pixel)
        };
    }
}