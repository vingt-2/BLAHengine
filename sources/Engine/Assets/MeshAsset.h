#pragma once
#include "Asset.h"

#include <Engine/Geometry/TriangleMesh.h>

namespace BLAengine
{
    class BLACORE_API MeshAsset : public Asset
    {
    public:
        MeshAsset(string name) : Asset(name) {}

        TriangleMesh m_triangleMesh;
    };
}