// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Maths/Maths.h"

namespace BLA
{
    class SkeletonAnimationData;

    class BVHImport
    {
    public:
        static blaVector<SkeletonAnimationData*> ImportAnimation(blaString bvhFilePath);
    };
}