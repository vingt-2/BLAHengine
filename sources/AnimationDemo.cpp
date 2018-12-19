#include "./Common/System.h"
#include "./Common/Maths/Maths.h"
#include "./Engine/Renderer/GL33Renderer.h"
#include "./Engine/Game/RenderingManager.h"
#include "./Engine/Game/Debug.h"
#include "./Engine/Assets/SceneManager.h"
#include "./Engine/Editor/CameraControl.h"
#include "Engine/Game/GameComponents/AnimationComponent.h"
#include "./AssetsImport/ExternalFormats/BVHImport.h"

#include "AnimationDemo.h"

using namespace BLAengine;


void AnimationDemo::PreEngineUpdate()
{
    EngineInstance::PreEngineUpdate();

    GameObject* animationObject = m_workingScene->FindObjectByName("AnimatedObject");
    
    if (animationObject != nullptr)
    {
        if (auto animCmp = animationObject->GetComponent<AnimationComponent>())
        {
            if (animCmp->m_animation == nullptr)
            {
                animCmp->m_animation = BVHImport::ImportAnimation("./resources/animations/bvh/01_02.bvh")[0];
            }
            else
            {
                blaF32 animDt = 1.0f / animCmp->m_animation->GetSamplingRate();

                if (m_renderWindow->GetKeyPressed(GLFW_KEY_RIGHT))
                {
                    m_frameIndex += 0.5f;
                }
                if (m_renderWindow->GetKeyPressed(GLFW_KEY_LEFT))
                {
                    m_frameIndex -= 0.5f;
                    m_frameIndex = m_frameIndex < 0.f ? 0.f : m_frameIndex;
                }

                m_frameIndex = m_frameIndex >= animCmp->m_animation->GetFrameCount() ? 0.f : m_frameIndex;

                vector<blaPosQuat> jointTransformsW;
                animCmp->m_animation->EvaluateAnimation(static_cast<int>(m_frameIndex), jointTransformsW);

                std::vector<std::pair<blaVec3, blaVec3>> bones;
                
                SkeletonAnimationData::GetBoneArrayFromEvalAnim(bones, animCmp->m_animation->GetSkeleton(), jointTransformsW);

                for (size_t i = 0; i < bones.size(); ++i)
                {
                    m_debug->DrawLine(bones[i].first, bones[i].second);
                }
            }
        }
    }

    if (m_cameraController)
    {
        m_cameraController->UpdateController();
    }
    
    //m_debug->DrawGrid(1000, 10, blaVec3(0.4f));

    ObjectTransform identity;
    m_debug->DrawBasis(&identity,1.f);
    m_debug->Update();
    m_timer->Update();
    m_workingScene->Update();
    m_renderer->Update();

}

bool AnimationDemo::LoadWorkingScene(std::string filepath)
{
    EngineInstance::LoadWorkingScene(filepath);

    GameObject* animatedObject = m_workingScene->CreateObject("AnimatedObject");
    AnimationComponent* animationCmp = BLA_CREATE_COMPONENT(AnimationComponent, animatedObject);

    m_cameraController = new CameraController(m_renderWindow, m_workingScene->GetMainCamera());

    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(1);
    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(2);
    
    return true;
}

void AnimationDemo::TerminateEngine()
{
    EngineInstance::TerminateEngine();
}
