using namespace std;

#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>    



#include <Engine/Assets/Texture.h>

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

    glm::uint8 m_nComponents;
    glm::uint32 m_dataSize;
    glm::uint32 m_width, m_height;

    std::string m_name;
    std::vector<glm::uint8> m_data;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            m_name, m_nComponents, m_dataSize, m_width, m_height, m_data
        );
    }
};
