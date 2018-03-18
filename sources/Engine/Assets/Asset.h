#pragma once
#include "../../Common/StdInclude.h"

namespace BLAengine
{
    class BLACORE_API Asset
    {
    public:

        Asset(std::string assetname)
        {
            m_assetName = assetname;
            m_isDirty = true;
        }

        std::string GetName() { return m_assetName; }

        bool IsDirty() { return m_isDirty; }
    private:

        bool m_isDirty;
        std::string m_assetName;
    };
}