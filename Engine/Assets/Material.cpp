#include "Material.h"
using namespace BLA;

bool Material::AssignTexture(blaString textureName, blaString handleName)
{
    blaString texture = blaString(textureName);
    blaString handle = blaString(handleName);
    blaPair<blaString, blaString> entry = blaPair<blaString, blaString>(texture, handle);
    m_textureSamplerAttributes.push_back(entry);

    return true;
}

