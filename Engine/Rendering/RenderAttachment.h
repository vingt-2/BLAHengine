// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "BLAStringID.h"
#include "StdInclude.h"
#include "Core/InspectableVariables.h"
#include "BLASingleton.h"

#include "Gpu/Image.h"

namespace BLA
{
	template<typename attachmentDataType> // <- The data type describing each entry in the attachment buffer // Also determines sample count for vk ?
	struct RenderAttachment
	{
		RenderAttachment<attachmentDataType>(Gpu::Image &image) : m_image(&image)
		{}
		
		Gpu::Image* m_image;
	};
}