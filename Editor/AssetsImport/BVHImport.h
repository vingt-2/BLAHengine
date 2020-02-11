#pragma once
#include <StdInclude.h>
#include <Maths/Maths.h>

namespace BLAengine
{
    class SkeletonAnimationData;

    class BVHImport
    {
    public:
        static blaVector<SkeletonAnimationData*> ImportAnimation(blaString bvhFilePath);
    };
}