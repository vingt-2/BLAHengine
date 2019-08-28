#pragma once
#include <Engine/Core/GameComponent.h>

#include "SkeletonAnimationData.h"

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(AnimationComponent)
        void Update() override;

        SkeletonAnimationData* m_animation;

        AnimationComponent(GameObjectReference parentObject);
        ~AnimationComponent(void) override;
    END_DECLARATION()
}
