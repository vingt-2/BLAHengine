#pragma once
#include "Asset.h"

#include <Geometry/TriangleMesh.h>

namespace BLAengine
{
    class BLACORE_API MeshAsset : public Asset
    {
    public:
        MeshAsset(blaString name) : Asset(name) {}

        TriangleMesh m_triangleMesh;
    };
}