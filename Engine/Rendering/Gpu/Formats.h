// BLAEngine Copyright (C, UndefinedAssociatedStruct) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "Maths/Maths.h"

#define DECLARE_FORMAT_STRUCT(Name, IndexName, AssociatedStructName, UndefinedAssociatedStruct)    \
    struct Name : Format                                                    \
    {                                                                       \
        Name() : Format(Enum::Index::IndexName) {}                          \
        static const Enum::Index ms_formatIndex = Enum::Index::IndexName;   \
        using AssociatedType = AssociatedStructName;                        \
    };

namespace BLA
{
    namespace Gpu
    {
        namespace Formats
        {
            namespace Enum
            {
                enum Index : blaU32
                {
                    R4G4_UNORM_PACK8,
                    R4G4B4A4_UNORM_PACK16,
                    B4G4R4A4_UNORM_PACK16,
                    R5G6B5_UNORM_PACK16,
                    B5G6R5_UNORM_PACK16,
                    R5G5B5A1_UNORM_PACK16,
                    B5G5R5A1_UNORM_PACK16,
                    A1R5G5B5_UNORM_PACK16,
                    R8_UNORM,
                    R8_SNORM,
                    R8_USCALED,
                    R8_SSCALED,
                    R8_UINT,
                    R8_SINT,
                    R8_SRGB,
                    R8G8_UNORM,
                    R8G8_SNORM,
                    R8G8_USCALED,
                    R8G8_SSCALED,
                    R8G8_UINT,
                    R8G8_SINT,
                    R8G8_SRGB,
                    R8G8B8_UNORM,
                    R8G8B8_SNORM,
                    R8G8B8_USCALED,
                    R8G8B8_SSCALED,
                    R8G8B8_UINT,
                    R8G8B8_SINT,
                    R8G8B8_SRGB,
                    B8G8R8_UNORM,
                    B8G8R8_SNORM,
                    B8G8R8_USCALED,
                    B8G8R8_SSCALED,
                    B8G8R8_UINT,
                    B8G8R8_SINT,
                    B8G8R8_SRGB,
                    R8G8B8A8_UNORM,
                    R8G8B8A8_SNORM,
                    R8G8B8A8_USCALED,
                    R8G8B8A8_SSCALED,
                    R8G8B8A8_UINT,
                    R8G8B8A8_SINT,
                    R8G8B8A8_SRGB,
                    B8G8R8A8_UNORM,
                    B8G8R8A8_SNORM,
                    B8G8R8A8_USCALED,
                    B8G8R8A8_SSCALED,
                    B8G8R8A8_UINT,
                    B8G8R8A8_SINT,
                    B8G8R8A8_SRGB,
                    A8B8G8R8_UNORM_PACK32,
                    A8B8G8R8_SNORM_PACK32,
                    A8B8G8R8_USCALED_PACK32,
                    A8B8G8R8_SSCALED_PACK32,
                    A8B8G8R8_UINT_PACK32,
                    A8B8G8R8_SINT_PACK32,
                    A8B8G8R8_SRGB_PACK32,
                    A2R10G10B10_UNORM_PACK32,
                    A2R10G10B10_SNORM_PACK32,
                    A2R10G10B10_USCALED_PACK32,
                    A2R10G10B10_SSCALED_PACK32,
                    A2R10G10B10_UINT_PACK32,
                    A2R10G10B10_SINT_PACK32,
                    A2B10G10R10_UNORM_PACK32,
                    A2B10G10R10_SNORM_PACK32,
                    A2B10G10R10_USCALED_PACK32,
                    A2B10G10R10_SSCALED_PACK32,
                    A2B10G10R10_UINT_PACK32,
                    A2B10G10R10_SINT_PACK32,
                    R16_UNORM,
                    R16_SNORM,
                    R16_USCALED,
                    R16_SSCALED,
                    R16_UINT,
                    R16_SINT,
                    R16_SFLOAT,
                    R16G16_UNORM,
                    R16G16_SNORM,
                    R16G16_USCALED,
                    R16G16_SSCALED,
                    R16G16_UINT,
                    R16G16_SINT,
                    R16G16_SFLOAT,
                    R16G16B16_UNORM,
                    R16G16B16_SNORM,
                    R16G16B16_USCALED,
                    R16G16B16_SSCALED,
                    R16G16B16_UINT,
                    R16G16B16_SINT,
                    R16G16B16_SFLOAT,
                    R16G16B16A16_UNORM,
                    R16G16B16A16_SNORM,
                    R16G16B16A16_USCALED,
                    R16G16B16A16_SSCALED,
                    R16G16B16A16_UINT,
                    R16G16B16A16_SINT,
                    R16G16B16A16_SFLOAT,
                    R32_UINT,
                    R32_SINT,
                    R32_SFLOAT,
                    R32G32_UINT,
                    R32G32_SINT,
                    R32G32_SFLOAT,
                    R32G32B32_UINT,
                    R32G32B32_SINT,
                    R32G32B32_SFLOAT,
                    R32G32B32A32_UINT,
                    R32G32B32A32_SINT,
                    R32G32B32A32_SFLOAT,
                    R64_UINT,
                    R64_SINT,
                    R64_SFLOAT,
                    R64G64_UINT,
                    R64G64_SINT,
                    R64G64_SFLOAT,
                    R64G64B64_UINT,
                    R64G64B64_SINT,
                    R64G64B64_SFLOAT,
                    R64G64B64A64_UINT,
                    R64G64B64A64_SINT,
                    R64G64B64A64_SFLOAT,
                    B10G11R11_UFLOAT_PACK32,
                    E5B9G9R9_UFLOAT_PACK32,
                    D16_UNORM,
                    X8_D24_UNORM_PACK32,
                    D32_SFLOAT,
                    S8_UINT,
                    D16_UNORM_S8_UINT,
                    D24_UNORM_S8_UINT,
                    D32_SFLOAT_S8_UINT
                };
            }

            struct Format
            {
                Format(Enum::Index index): m_index(index) {}

                Enum::Index m_index;
            };

            struct UndefinedAssociatedStruct
            {
                void* m_storage[4];
            };

            DECLARE_FORMAT_STRUCT(R8_UNORM, R8_UNORM, blaU8)
            DECLARE_FORMAT_STRUCT(R8_SNORM, R8_SNORM, blaS8)
            DECLARE_FORMAT_STRUCT(R8_USCALED, R8_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8_SSCALED, R8_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8_UINT, R8_UINT, blaU8)
            DECLARE_FORMAT_STRUCT(R8_SINT, R8_SINT, blaS8)
            DECLARE_FORMAT_STRUCT(R8_SRGB, R8_SRGB, blaS8)
            DECLARE_FORMAT_STRUCT(R8G8_UNORM, R8G8_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8_SNORM, R8G8_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8_USCALED, R8G8_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8_SSCALED, R8G8_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8_UINT, R8G8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8_SINT, R8G8_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8_SRGB, R8G8_SRGB, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8_UNORM, R8G8B8_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8_SNORM, R8G8B8_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8_USCALED, R8G8B8_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8_SSCALED, R8G8B8_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8_UINT, R8G8B8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8_SINT, R8G8B8_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8_SRGB, R8G8B8_SRGB, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8_UNORM, B8G8R8_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8_SNORM, B8G8R8_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8_USCALED, B8G8R8_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8_SSCALED, B8G8R8_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8_UINT, B8G8R8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8_SINT, B8G8R8_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8_SRGB, B8G8R8_SRGB, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8A8_UNORM, R8G8B8A8_UNORM, blaU32)
            DECLARE_FORMAT_STRUCT(R8G8B8A8_SNORM, R8G8B8A8_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8A8_USCALED, R8G8B8A8_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8A8_SSCALED, R8G8B8A8_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8A8_UINT, R8G8B8A8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8A8_SINT, R8G8B8A8_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R8G8B8A8_SRGB, R8G8B8A8_SRGB, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8A8_UNORM, B8G8R8A8_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8A8_SNORM, B8G8R8A8_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8A8_USCALED, B8G8R8A8_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8A8_SSCALED, B8G8R8A8_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8A8_UINT, B8G8R8A8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8A8_SINT, B8G8R8A8_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B8G8R8A8_SRGB, B8G8R8A8_SRGB, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16_UNORM, R16_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16_SNORM, R16_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16_USCALED, R16_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16_SSCALED, R16_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16_UINT, R16_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16_SINT, R16_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16_SFLOAT, R16_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16_UNORM, R16G16_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16_SNORM, R16G16_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16_USCALED, R16G16_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16_SSCALED, R16G16_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16_UINT, R16G16_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16_SINT, R16G16_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16_SFLOAT, R16G16_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16_UNORM, R16G16B16_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16_SNORM, R16G16B16_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16_USCALED, R16G16B16_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16_SSCALED, R16G16B16_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16_UINT, R16G16B16_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16_SINT, R16G16B16_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16_SFLOAT, R16G16B16_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16A16_UNORM, R16G16B16A16_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16A16_SNORM, R16G16B16A16_SNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16A16_USCALED, R16G16B16A16_USCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16A16_SSCALED, R16G16B16A16_SSCALED, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16A16_UINT, R16G16B16A16_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16A16_SINT, R16G16B16A16_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R16G16B16A16_SFLOAT, R16G16B16A16_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32_UINT, R32_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32_SINT, R32_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32_SFLOAT, R32_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32G32_UINT, R32G32_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32G32_SINT, R32G32_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32G32_SFLOAT, R32G32_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32G32B32_UINT, R32G32B32_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32G32B32_SINT, R32G32B32_SINT, blaIVec3)
            DECLARE_FORMAT_STRUCT(R32G32B32_SFLOAT, R32G32B32_SFLOAT, blaVec3)
            DECLARE_FORMAT_STRUCT(R32G32B32A32_UINT, R32G32B32A32_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R32G32B32A32_SINT, R32G32B32A32_SINT, blaIVec4)
            DECLARE_FORMAT_STRUCT(R32G32B32A32_SFLOAT, R32G32B32A32_SFLOAT, blaVec4)
            DECLARE_FORMAT_STRUCT(R64_UINT, R64_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64_SINT, R64_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64_SFLOAT, R64_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64_UINT, R64G64_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64_SINT, R64G64_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64_SFLOAT, R64G64_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64B64_UINT, R64G64B64_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64B64_SINT, R64G64B64_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64B64_SFLOAT, R64G64B64_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64B64A64_UINT, R64G64B64A64_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64B64A64_SINT, R64G64B64A64_SINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R64G64B64A64_SFLOAT, R64G64B64A64_SFLOAT, UndefinedAssociatedStruct)

            DECLARE_FORMAT_STRUCT(R4G4_UNORM_PACK8, R4G4_UNORM_PACK8, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R4G4B4A4_UNORM_PACK16, R4G4B4A4_UNORM_PACK16, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B4G4R4A4_UNORM_PACK16, B4G4R4A4_UNORM_PACK16, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R5G6B5_UNORM_PACK16, R5G6B5_UNORM_PACK16, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B5G6R5_UNORM_PACK16, B5G6R5_UNORM_PACK16, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(R5G5B5A1_UNORM_PACK16, R5G5B5A1_UNORM_PACK16, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B5G5R5A1_UNORM_PACK16, B5G5R5A1_UNORM_PACK16, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A1R5G5B5_UNORM_PACK16, A1R5G5B5_UNORM_PACK16, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A8B8G8R8_UNORM_PACK32, A8B8G8R8_UNORM_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A8B8G8R8_SNORM_PACK32, A8B8G8R8_SNORM_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A8B8G8R8_USCALED_PACK32, A8B8G8R8_USCALED_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A8B8G8R8_SSCALED_PACK32, A8B8G8R8_SSCALED_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A8B8G8R8_UINT_PACK32, A8B8G8R8_UINT_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A8B8G8R8_SINT_PACK32, A8B8G8R8_SINT_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A8B8G8R8_SRGB_PACK32, A8B8G8R8_SRGB_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2R10G10B10_UNORM_PACK32, A2R10G10B10_UNORM_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2R10G10B10_SNORM_PACK32, A2R10G10B10_SNORM_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2R10G10B10_USCALED_PACK32, A2R10G10B10_USCALED_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2R10G10B10_SSCALED_PACK32, A2R10G10B10_SSCALED_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2R10G10B10_UINT_PACK32, A2R10G10B10_UINT_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2R10G10B10_SINT_PACK32, A2R10G10B10_SINT_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2B10G10R10_UNORM_PACK32, A2B10G10R10_UNORM_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2B10G10R10_SNORM_PACK32, A2B10G10R10_SNORM_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2B10G10R10_USCALED_PACK32, A2B10G10R10_USCALED_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2B10G10R10_SSCALED_PACK32, A2B10G10R10_SSCALED_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2B10G10R10_UINT_PACK32, A2B10G10R10_UINT_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(A2B10G10R10_SINT_PACK32, A2B10G10R10_SINT_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(B10G11R11_UFLOAT_PACK32, B10G11R11_UFLOAT_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(E5B9G9R9_UFLOAT_PACK32, E5B9G9R9_UFLOAT_PACK32, UndefinedAssociatedStruct)

            DECLARE_FORMAT_STRUCT(D16_UNORM, D16_UNORM, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(X8_D24_UNORM_PACK32, X8_D24_UNORM_PACK32, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(D32_SFLOAT, D32_SFLOAT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(S8_UINT, S8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(D16_UNORM_S8_UINT, D16_UNORM_S8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(D24_UNORM_S8_UINT, D24_UNORM_S8_UINT, UndefinedAssociatedStruct)
            DECLARE_FORMAT_STRUCT(D32_SFLOAT_S8_UINT, D32_SFLOAT_S8_UINT, UndefinedAssociatedStruct)

        }
    }
};