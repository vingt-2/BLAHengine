#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>

#include <Engine/Assets/Material.h>

class MaterialSerializer
{
public:

    MaterialSerializer() = default;

    void FromMaterial(BLAengine::Material* material)
    {
        m_name = material->GetName();
        for (auto texturePair : material->m_textureSamplerAttributes)
        {
            m_textureNames.push_back(texturePair.first);
            m_textureHandles.push_back(texturePair.second);
        }
    }
    BLAengine::Material* BuildMaterial()
    {
        if (m_textureNames.size() != m_textureHandles.size())
            return nullptr;

        BLAengine::Material* mat = new BLAengine::Material(m_name);
        
        for (size_t i = 0; i < m_textureHandles.size(); i++)
        {
            std::string texName = m_textureNames[i];
            std::string texHandle = m_textureHandles[i];
            mat->m_textureSamplerAttributes.push_back(std::pair<std::string, std::string>(texName, texHandle));
        }
        return mat;
    }

private:
    friend class cereal::access;

    std::string m_name;

    std::vector<std::string> m_textureNames;
    std::vector<std::string> m_textureHandles;


    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            m_name, m_textureNames, m_textureHandles
        );
    }
};
