// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "AnimationComponent.h"
#include "Core/TransformComponent.h"
#include "EngineInstance.h"
#include "System/InputManager.h"
#include "Core/Timer.h"
#include "Core/DebugDraw.h"
#include "Core/Scene.h"

using namespace BLA;

BeginBehaviorDescription(AnimationComponent, Dependencies(RootSystem))
Expose(m_play)
Expose(m_frameIndex)
Expose(m_bvhName)
Expose(m_playbackMultiplier)
Expose(m_scaleFactor)
EndBehaviorDescription()

void AnimationComponent::Update()
{
    InputManager* inputs = InputManager::GetSingletonInstance();
    if (m_animation != nullptr)
    {
        blaF32 animDt = 1.0f / m_animation->GetSamplingRate();
        const EngineInstance* engine = EngineInstance::GetSingletonInstanceRead();

        blaF32 gameDt = Core::Scene::GetSingletonInstance()->m_sceneTimer.GetDelta();
        blaF32 animStep = m_playbackMultiplier * (gameDt / animDt);

        if (inputs->GetKeyState(BLA_KEY_RIGHT).IsDown() || inputs->GetKeyState(BLA_KEY_LEFT).IsDown())
        {
            m_play = false;
        }

        if (m_play)
        {
            m_frameIndex += animStep;
        }
        else
        {
            if (inputs->GetKeyState(BLA_KEY_RIGHT).IsDown())
            {
                m_frameIndex += animStep;
                m_lastTimePlayerInteraction = Core::Scene::GetSingletonInstance()->m_sceneTimer.GetTime();
            }
            if (inputs->GetKeyState(BLA_KEY_LEFT).IsDown())
            {
                m_frameIndex -= animStep;
                m_frameIndex = m_frameIndex < 0.f ? 0.f : m_frameIndex;
                m_lastTimePlayerInteraction = Core::Scene::GetSingletonInstance()->m_sceneTimer.GetTime();
            }
        }

        m_frameIndex = m_frameIndex >= m_animation->GetFrameCount() ? 0.f : m_frameIndex;

        blaVector<blaPosQuat> jointTransformsW;
        m_animation->EvaluateAnimation(static_cast<int>(m_frameIndex), jointTransformsW);

        /*for (auto jointW : jointTransformsW)
        {
            DebugDraw::DrawBasis(jointW, 1.f);
        }*/

        blaVector<blaPair<blaPosQuat, blaF32>> bones;

        SkeletonAnimationData::GetBoneArrayFromEvalAnim(bones, m_animation->GetSkeleton(), jointTransformsW);

        blaVec3 origin = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetPosition();

        for (size_t i = 0; i < bones.size(); ++i)
        {
            blaPosQuat t = bones[i].first;
            blaF32 h = bones[i].second;

            t.SetTranslation3(origin + m_scaleFactor * t.GetTranslation3());
            h = 0.3f;

            DebugDraw::DrawPlainOBB(t, blaVec3(0.5f * h, 0.1f, 0.1f), blaVec4(BLA::ORANGE, 0.6f));
            DebugDraw::DrawOBB(t, blaVec3(0.5f * h, 0.1f, 0.1f), blaVec3(0));
        }
    }

    if (m_bvhName != m_prevBVHName)
    {
        delete m_animation;

        //blaVector<SkeletonAnimationData*> anims = BVHImport::ImportAnimation(m_bvhName); // <- This needs to be imported in the editor

        //if (!anims.empty())
        //    m_animation = anims[0];
        //else
            m_animation = nullptr;
    }

    m_prevBVHName = m_bvhName;
}

void AnimationComponent::Shutdown()
{
    delete m_animation;
}
