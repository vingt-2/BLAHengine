#pragma once

#include "Geometry/TriangleMesh.h"
#include "glm/common.hpp"

namespace BLA
{
    class BLACORE_API PrimitiveGeometry
    {
    public:
        static TriangleMesh MakeCube();
        static TriangleMesh MakeSphere(blaF32 radius, bool inverted = false, blaU32 thetaSamples = 32, blaU32 phiSamples = 32);
        static TriangleMesh MakeDisc(blaU32 resolution);
        static TriangleMesh MakeCone(blaU32 resolution);
		static TriangleMesh MakePlane(blaVec3 normal, blaU32 width, blaU32 height, blaU32 subdivisionDepth);
    };
};