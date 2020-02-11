#pragma once
#include "StdInclude.h"

namespace BLAengine
{
    class BLACORE_API Asset
    {
    public:

        Asset(blaString assetname)
        {
            m_assetName = assetname;
            m_isDirty = true;
        }

        blaString GetName() { return m_assetName; }

        bool IsDirty() { return m_isDirty; }
    private:

        bool m_isDirty;
        blaString m_assetName;
    };
}