#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>

namespace BLAengine
{
    class SkeletonAnimationData;

    class BLACORE_API BVHImport
    {
    public:
        static blaVector<SkeletonAnimationData*> ImportAnimation(blaString bvhFilePath);
    };
}