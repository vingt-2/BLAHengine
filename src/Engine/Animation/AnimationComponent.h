#pragma once
#include <Engine/Core/GameComponent.h>

#include "SkeletonAnimationData.h"

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(AnimationComponent)
        void Update() override;

        SkeletonAnimationData* m_animation;

		bool m_play;
		blaF32 m_frameIndex;
		blaString m_bvhName;
		blaF32 m_playbackMultiplier;

        AnimationComponent(GameObjectReference parentObject);
        ~AnimationComponent(void) override;
	private:
		blaF32 m_lastTimePlayerInteraction;
		blaString m_prevBVHName;
    END_DECLARATION()
}
