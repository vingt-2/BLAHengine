#pragma once
#include "../../Common/StdInclude.h"
#include "../../Common/Maths/Maths.h"

class SkeletalMotion;

namespace BLAengine
{
    class BLACORE_API BVHImport
    {
    public:
        static SkeletalMotion* ImportAnimation(string bvhFilePath);
    };
}