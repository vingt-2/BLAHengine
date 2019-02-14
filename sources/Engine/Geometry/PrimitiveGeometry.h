#pragma once

#include <Engine/Geometry/TriangleMesh.h>

namespace BLAengine
{
    class PrimitiveGeometry
    {
    public:
        static TriangleMesh MakeCube();
        static TriangleMesh MakeSphere(blaF32 radius);
        static TriangleMesh MakeDisc(blaU32 resolution);
        static TriangleMesh MakeCone(blaU32 resolution);
    };
};