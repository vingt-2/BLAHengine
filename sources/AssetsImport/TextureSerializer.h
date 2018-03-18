#include "../Engine/Assets/Texture.h"
#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>

class Texture2DSerializer
{
public:
        
    Texture2DSerializer() = default;

    void FromTexture(BLAengine::Texture2D* texture) 
    {
        m_name = texture->GetName();
        m_nComponents = texture->m_nComponents;
        m_dataSize = texture->m_dataSize;
        m_width = texture->m_width;
        m_height = texture->m_height;
        m_data = texture->m_data;
    }
    BLAengine::Texture2D* BuildTexture() 
    {
        return new BLAengine::Texture2D(m_name, m_nComponents, m_data, m_width, m_height);
    }

private:
    friend class cereal::access;

    uint8_t m_nComponents;
    uint32_t m_dataSize;
    uint32_t m_width, m_height;

    std::string m_name;
    std::vector<uint8_t> m_data;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            m_name, m_nComponents, m_dataSize, m_width, m_height, m_data
        );
    }
};
