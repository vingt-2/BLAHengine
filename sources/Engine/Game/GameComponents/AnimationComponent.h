#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>
#include <Engine/Game/Animation/SkeletonAnimationData.h>

#include "GameComponent.h"

namespace BLAengine
{
    class BLACORE_API AnimationComponent : public GameComponent
    {
    public:

        void Update() override;

        SkeletonAnimationData* m_animation;

        AnimationComponent(GameObject* parentObject);
        ~AnimationComponent(void);

    private:
    };
}
