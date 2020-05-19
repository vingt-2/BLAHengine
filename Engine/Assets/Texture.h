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

        //TODO: Support mipMapping 
        Texture2D(blaString name, glm::uint8 nComponents, blaVector<glm::uint8> data, blaU32 width, blaU32 height);
        ~Texture2D(void);

        glm::uint8 m_nComponents;
        blaU32 m_dataSize;
        blaU32 m_width, m_height;
        blaBool m_hasAlpha;

        blaVector<glm::uint8> m_data;

        blaBool HasAlpha() const { return m_nComponents > 3; }

    private:
    };

    class TextureImport
    {
    public:

    	// TODO: Move to editor ...
        BLACORE_API static Texture2D* LoadBMP(blaString name, blaString filePath);
        BLACORE_API static Texture2D* LoadDDS(blaString name, blaString filePath);
        BLACORE_API static Texture2D* LoadTGA(blaString name, blaString filepath);
    };
}