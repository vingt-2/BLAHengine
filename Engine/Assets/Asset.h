#pragma once
#include "StdInclude.h"

namespace BLA
{
    class BLACORE_API Asset
    {
    public:

        Asset(blaString assetname)
        {
            m_assetName = assetname;
            m_isDirty = true;
        }

        blaString GetName() const { return m_assetName; }

        bool IsDirty() const { return m_isDirty; }
    private:

        bool m_isDirty;
        blaString m_assetName;
    };
}