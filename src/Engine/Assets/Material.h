#pragma once
#include <Common/StdInclude.h>
#include <Common/System.h>
#include <Common/Maths/Maths.h>

#include "Texture.h"

namespace BLAengine
{
    class BLACORE_API Material : public Asset
    {
    public:
        Material(blaString name) : Asset(name)
        {};

        bool AssignTexture(blaString textureName, blaString handleName);

        blaVector<blaPair<blaString, blaString>> m_textureSamplerAttributes;
    };
}