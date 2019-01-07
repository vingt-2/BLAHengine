#pragma once
#include "../../Common/StdInclude.h"
#include "../../Common/Maths/Maths.h"

namespace BLAengine
{
    class SkeletonAnimationData;

    class BLACORE_API BVHImport
    {
    public:
        static vector<SkeletonAnimationData*> ImportAnimation(string bvhFilePath);
    };
}