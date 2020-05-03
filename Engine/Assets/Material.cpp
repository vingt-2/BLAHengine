#include "Material.h"
using namespace BLA;

blaVector<blaPair<blaString, blaString>>& Material::GetSamplers()
{
    return m_textureSamplerAttributes;
}

bool Material::AssignTexture(blaString textureName, blaString handleName)
{
    blaString texture = blaString(textureName);
    blaString handle = blaString(handleName);
    blaPair<blaString, blaString> entry = blaPair<blaString, blaString>(texture, handle);
    m_textureSamplerAttributes.push_back(entry);

    return true;
}

