#pragma once
#include "../../Common/StdInclude.h"
#include "../../Common/Maths/Maths.h"

class SkeletonAnimationData;

namespace BLAengine
{
    class BLACORE_API BVHImport
    {
    public:
        static vector<SkeletonAnimationData*> ImportAnimation(string bvhFilePath);
    };
}