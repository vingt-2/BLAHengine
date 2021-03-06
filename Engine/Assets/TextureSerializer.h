// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

using namespace std;

#include "cereal\cereal.hpp"
#include "cereal\types\vector.hpp"
#include "cereal\types\string.hpp"
#include "cereal\archives\binary.hpp"    



#include "Assets/Texture.h"

class Texture2DSerializer
{
public:

    Texture2DSerializer() = default;

    void FromTexture(BLA::Texture2D* texture)
    {
        m_name = texture->GetName();
        m_nComponents = texture->m_nComponents;
        m_dataSize = texture->m_dataSize;
        m_width = texture->m_width;
        m_height = texture->m_height;
        m_data = texture->m_data;
    }
    BLA::Texture2D* BuildTexture()
    {
        return new BLA::Texture2D(m_name, m_nComponents, m_data, m_width, m_height);
    }

private:
    friend class cereal::access;

    glm::uint8 m_nComponents;
    blaU32 m_dataSize;
    blaU32 m_width, m_height;

    blaString m_name;
    blaVector<glm::uint8> m_data;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            m_name, m_nComponents, m_dataSize, m_width, m_height, m_data
        );
    }
};
