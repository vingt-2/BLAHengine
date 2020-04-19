#pragma once
#include "Core/GameComponent.h"

#include "SkeletonAnimationData.h"

namespace BLAengine
{
    BeginBehaviorDeclaration(BLAEngineNative, AnimationComponent)
        void Shutdown() override;

        SkeletonAnimationData* m_animation = nullptr;

        bool m_play = false;
        blaF32 m_frameIndex = 0.f;
		blaString m_bvhName;
		blaF32 m_playbackMultiplier = 1.f;
		blaF32 m_scaleFactor = 0.1f;

	private:
		blaF32 m_lastTimePlayerInteraction = 0.f;
		blaString m_prevBVHName;
    EndBehaviorDeclaration()
}
