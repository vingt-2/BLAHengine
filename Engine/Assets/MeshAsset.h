// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "Asset.h"

#include "Geometry/TriangleMesh.h"

namespace BLA
{
    class MeshAsset : public Asset
    {
    public:
        BLACORE_API MeshAsset(blaString name) : Asset(name) {}

        TriangleMesh m_triangleMesh;
    };
}