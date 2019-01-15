#include <Common/System.h>
#include <Common/Maths/Maths.h>
#include <Engine/Renderer/GL33Renderer.h>
#include <Engine/Game/RenderingManager.h>
#include <Engine/Game/Debug.h>
#include <Engine/Assets/SceneManager.h>
#include <Editor/CameraControl.h>
#include <Engine/Game/GameComponents/AnimationComponent.h>
#include <AssetsImport/ExternalFormats/BVHImport.h>
#include <Engine/Game/GameComponents/IKComponent.h>
#include <Engine/Game/Animation/IK/IKTools.h>
#include <Engine/System/InputManager.h>

#include "AnimationDemo.h"

using namespace BLAengine;

int a = 0;
void AnimationDemo::PreEngineUpdate()
{
    EngineInstance::PreEngineUpdate();

    const InputManager* inputs = InputManager::GetSingletonInstanceRead();

    GameObject* animationObject = m_workingScene->FindObjectByName("AnimatedObject");
    
    if (animationObject != nullptr)
    {
        if (auto ikCmp = animationObject->GetComponent<IKComponent>())
        {
            if (ikCmp->m_ikChain == nullptr)
            {
                if (auto animCmp = animationObject->GetComponent<AnimationComponent>())
                {
                    if (animCmp->m_animation == nullptr)
                    {
                        animCmp->m_animation = BVHImport::ImportAnimation("./resources/animations/bvh/01_04.bvh")[0];
                        animCmp->m_animation->GetSkeleton()->DiscardJointsByName("Thumb");
                    }

                    vector<blaPosQuat> jointTransformsW;
                    animCmp->m_animation->EvaluateAnimation(0, jointTransformsW);

                   ikCmp->m_ikChain = IKChainJoint::BuildFromSkeleton(animCmp->m_animation->GetSkeleton(), jointTransformsW);
                }
                else
                {
                    ikCmp->m_ikChain = IKChainJoint::CreateTestIKChain2EndsConeTwist(4, 1.f, blaVec3(1.f, 2.f, 0.f));
                }

                int c = 0;
                for (auto endEffector : ikCmp->m_ikChain->GetEndEffectors())
                {
                    GameObject* object;
                    if(endEffector->m_joint != nullptr)
                    {
                        object = m_workingScene->CreateObject("EffectorHandles_" + endEffector->m_joint->GetName());
                    }
                    else
                    {
                        object = m_workingScene->CreateObject("EffectorHandles_" + to_string(c));
                    }

                    auto meshCmp = BLA_CREATE_COMPONENT(MeshRendererComponent, object);

                    Asset* sphereMesh = nullptr;
                    this->m_assetManager->GetAsset("sphere", sphereMesh);

                    meshCmp->AssignTriangleMesh((TriangleMesh*)sphereMesh);

                    auto meshCollider = BLA_CREATE_COMPONENT(MeshColliderComponent, object);

                    meshCollider->SetColliderMesh((TriangleMesh*)sphereMesh);

                    ObjectTransform transform = object->GetTransform();
                    transform.m_scale = blaVec3(0.3f);
                    transform.m_transform = endEffector->m_jointTransform;
                    object->SetTransform(transform);

                    c++;
                }
            }
            else
            {
                std::vector<std::pair<blaVec3, blaVec3>> bones;
                std::vector<blaPosQuat> jointTransforms;
                IKChainJoint::GetBoneArray(bones, *ikCmp->m_ikChain);
                IKChainJoint::GetJointTransforms(jointTransforms, *ikCmp->m_ikChain);

                vector<GameObject*> effectorHandles = m_workingScene->FindObjectsMatchingName("EffectorHandles_");
                vector<blaVec3> desiredPos;
                for (auto obj : effectorHandles)
                {
                    desiredPos.push_back(obj->GetTransform().GetPosition());
                }

                if (inputs->GetKeyState(BLA_KEY_SPACE).IsDown())
                {    
                    if (m_timer->GetTime() - m_lastIkSolveTime > .5f)
                    {
                        IKChainJoint::SolveIKChain(ikCmp->m_ikChain, desiredPos, 1);

                        m_lastIkSolveTime = m_timer->GetTime();
                    }
                }
                
                for (size_t i = 0; i < jointTransforms.size(); ++i)
                {
                    m_debug->DrawBasis(jointTransforms[i], 1.f);
                }

                for (size_t i = 0; i < bones.size(); ++i)
                {
                    m_debug->DrawLine(bones[i].first, bones[i].second, blaVec3(1.f, 0.f, 1.f));
                }
            }
        }
        else if (auto animCmp = animationObject->GetComponent<AnimationComponent>())
        {
            if (animCmp->m_animation == nullptr)
            {
                animCmp->m_animation = BVHImport::ImportAnimation("./resources/animations/bvh/01_04.bvh")[0];
            }
            else
            {
                blaF32 animDt = 1.0f / animCmp->m_animation->GetSamplingRate();
                const EngineInstance* engine = GetSingletonInstanceRead();
                
                blaF32 gameDt = engine->GetTimer()->GetDelta();
                blaF32 animStep = gameDt / animDt;

                if (inputs->GetKeyState(BLA_KEY_RIGHT).IsDown() || inputs->GetKeyState(BLA_KEY_LEFT).IsDown())
                {
                    m_autoPlay = false;
                }

                if(m_autoPlay)
                {
                    m_frameIndex += animStep;
                }
                else
                {
                    if (inputs->GetKeyState(BLA_KEY_RIGHT).IsDown())
                    {
                        m_frameIndex += animStep;
                        m_lastTimePlayerInteraction = engine->GetTimer()->GetTime();
                    }
                    if (inputs->GetKeyState(BLA_KEY_LEFT).IsDown())
                    {
                        m_frameIndex -= animStep;
                        m_frameIndex = m_frameIndex < 0.f ? 0.f : m_frameIndex;
                        m_lastTimePlayerInteraction = engine->GetTimer()->GetTime();
                    }

                    if(engine->GetTimer()->GetTime() - m_lastTimePlayerInteraction > 5.0f)
                    {
                        m_autoPlay = true;
                    }
                }

                m_frameIndex = m_frameIndex >= animCmp->m_animation->GetFrameCount() ? 0.f : m_frameIndex;

                vector<blaPosQuat> jointTransformsW;
                animCmp->m_animation->EvaluateAnimation(static_cast<int>(m_frameIndex), jointTransformsW);

                for(auto jointW : jointTransformsW)
                {
                    GetDebug()->DrawBasis(jointW,1.f);
                }

                std::vector<std::pair<blaVec3, blaVec3>> bones;
                
                SkeletonAnimationData::GetBoneArrayFromEvalAnim(bones, animCmp->m_animation->GetSkeleton(), jointTransformsW);

                for (size_t i = 0; i < bones.size(); ++i)
                {
                    m_debug->DrawLine(bones[i].first, bones[i].second);
                }
            }
        }
    }

    auto leftMouseButton = inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_LEFT);
    if (leftMouseButton.IsRisingEdge())
    {
        Ray screenRay = m_renderer->ScreenToRay(m_renderWindow->GetMousePointerScreenSpaceCoordinates());

        if (GameObject* object = m_workingScene->PickGameObjectInScene(screenRay).first)
        {
            m_selectedObject = object;
        }
    }
    if (leftMouseButton.IsFallingEdge())
    {
        m_selectedObject = nullptr;
    }


    if(m_selectedObject != nullptr) 
    {
        if(m_selectedObject->GetName().find("EffectorHandles_") != string::npos)
        {
            Ray screenRay = m_renderer->ScreenToRay(m_renderWindow->GetMousePointerScreenSpaceCoordinates());
            GameObject* camera = m_workingScene->GetMainCamera()->GetParentObject();

            blaVec3 camUp, camLeft;

            camUp = camera->GetTransform().GetRotation() * blaVec3(0.f, 1.f, 0.f);
            camLeft = camera->GetTransform().GetRotation() * blaVec3(1.f, 0.f, 0.f);

            RaycastHit hit = screenRay.RayToPlaneIntersection(m_selectedObject->GetTransform().GetPosition(), camUp, camLeft);

            ObjectTransform selectedObjectTransform = m_selectedObject->GetTransform();

            if (glm::length2(hit.hitPosition - selectedObjectTransform.GetPosition()) < 1.0f)
            {
                selectedObjectTransform.SetPosition(hit.hitPosition);

                m_selectedObject->SetTransform(selectedObjectTransform);
            }
        }
    }

    if (m_cameraController)
    {
        m_cameraController->UpdateController();
    }
    
    //m_debug->DrawGrid(1000, 10, blaVec3(0.4f));

    //m_debug->DrawBasis(blaPosQuat::GetIdentity() , 1.f);
}

bool AnimationDemo::LoadWorkingScene(std::string filepath)
{
    EngineInstance::LoadWorkingScene(filepath);

    GameObject* animatedObject = m_workingScene->CreateObject("AnimatedObject");

    BLA_CREATE_COMPONENT(AnimationComponent, animatedObject);

    BLA_CREATE_COMPONENT(IKComponent, animatedObject);

    m_cameraController = new CameraController(
        m_renderWindow, 
        m_workingScene->GetMainCamera(),
        40.f,
        10.0f);

    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(1);
    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(2);
    
    return true;
}

void AnimationDemo::TerminateEngine()
{
    EngineInstance::TerminateEngine();
}
