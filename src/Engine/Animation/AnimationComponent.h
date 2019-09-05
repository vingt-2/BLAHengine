#pragma once
#include <Engine/Core/GameComponent.h>

#include "SkeletonAnimationData.h"

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(AnimationComponent)
        void Update() override;

        SkeletonAnimationData* m_animation;

		bool m_play;
		blaS32 m_frameIndex;

        AnimationComponent(GameObjectReference parentObject);
        ~AnimationComponent(void) override;
	private:
		blaF32 m_lastTimePlayerInteraction;
    END_DECLARATION()
}
