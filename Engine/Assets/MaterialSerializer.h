#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>

#include <Assets/Material.h>

class MaterialSerializer
{
public:

    MaterialSerializer() = default;

    void FromMaterial(BLA::Material* material)
    {
        m_name = material->GetName();
        for (auto texturePair : material->m_textureSamplerAttributes)
        {
            m_textureNames.push_back(texturePair.first);
            m_textureHandles.push_back(texturePair.second);
        }
    }
    BLA::Material* BuildMaterial()
    {
        if (m_textureNames.size() != m_textureHandles.size())
            return nullptr;

        BLA::Material* mat = new BLA::Material(m_name);

        for (size_t i = 0; i < m_textureHandles.size(); i++)
        {
            blaString texName = m_textureNames[i];
            blaString texHandle = m_textureHandles[i];
            mat->m_textureSamplerAttributes.push_back(blaPair<blaString, blaString>(texName, texHandle));
        }
        return mat;
    }

private:
    friend class cereal::access;

    blaString m_name;

    blaVector<blaString> m_textureNames;
    blaVector<blaString> m_textureHandles;


    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            m_name, m_textureNames, m_textureHandles
        );
    }
};
