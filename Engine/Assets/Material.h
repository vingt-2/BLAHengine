#pragma once
#include <StdInclude.h>
#include <System.h>
#include <Maths/Maths.h>

#include "Texture.h"

namespace BLA
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