#pragma once
#include "Asset.h"

#include "StdInclude.h"
#include "System.h"
#include "Maths/Maths.h"

namespace BLA
{
    class Texture2D : public Asset
    {
    public:

        //TODO: Support mipblaMapping 
        Texture2D(blaString name, glm::uint8 nComponents, blaVector<glm::uint8> data, blaU32 width, blaU32 height);
        ~Texture2D(void);

        glm::uint8 m_nComponents;
        blaU32 m_dataSize;
        blaU32 m_width, m_height;

        blaVector<glm::uint8> m_data;

    private:
    };

    class TextureImport
    {
    public:

        static Texture2D* LoadBMP(blaString name, blaString filePath);
        static Texture2D* LoadDDS(blaString name, blaString filePath);
    };
}