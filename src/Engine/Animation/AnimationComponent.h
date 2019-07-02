#pragma once
#include <Engine/Core/GameComponent.h>

#include "SkeletonAnimationData.h"

namespace BLAengine
{
    class BLACORE_API AnimationComponent : public GameComponent
    {
    public:

        void Update() override;

        SkeletonAnimationData* m_animation;

        AnimationComponent(GameObjectReference parentObject);
        ~AnimationComponent(void) override;

    private:
    };
}
