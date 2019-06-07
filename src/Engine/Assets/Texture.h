#pragma once
#include "Asset.h"

#include <Common/StdInclude.h>
#include <Common/System.h>
#include <Common/Maths/Maths.h>

namespace BLAengine
{
    class BLACORE_API Texture2D : public Asset
    {
    public:

        //TODO: Support mipmapping 
        Texture2D(string name, glm::uint8 nComponents, vector<glm::uint8> data, blaU32 width, blaU32 height);
        ~Texture2D(void);

        glm::uint8 m_nComponents;
        blaU32 m_dataSize;
        blaU32 m_width, m_height;

        std::vector<glm::uint8> m_data;

    private:
    };

    class BLACORE_API TextureImport
    {
    public:

        static Texture2D* LoadBMP(string name, string filePath);
        static Texture2D* LoadDDS(string name, string filePath);
    };
}