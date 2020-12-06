// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "System.h"
#include "Maths/Maths.h"

#include "Texture.h"

namespace BLA
{
    class Material : public Asset
    {
        blaVector<blaPair<blaString, blaString>> m_textureSamplerAttributes;
        
    public:
        Material(blaString name) : Asset(name) {};

        const blaVector<blaPair<blaString, blaString>>& GetSamplers() const;
        blaVector<blaPair<blaString, blaString>>& GetSamplers();
        
        BLACORE_API bool AssignTexture(blaString textureName, blaString handleName);
    };
}