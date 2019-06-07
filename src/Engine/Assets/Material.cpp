#include "Material.h"
using namespace BLAengine;

bool Material::AssignTexture(std::string textureName, std::string handleName)
{
    string texture = string(textureName);
    string handle = string(handleName);
    pair<string, string> entry = pair<string, string>(texture, handle);
    m_textureSamplerAttributes.push_back(entry);

    return true;
}

