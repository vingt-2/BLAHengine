#pragma once
#include "Asset.h"

#include "Geometry/TriangleMesh.h"

namespace BLA
{
    class BLACORE_API MeshAsset : public Asset
    {
    public:
        MeshAsset(blaString name) : Asset(name) {}

        TriangleMesh m_triangleMesh;
    };
}