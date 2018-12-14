#include "AnimationDemo.h"
#include "Engine/Game/GameComponents/SimpleHairComponent.h"
#include "Engine/Game/GameComponents/AnimationComponent.h"
#include "./AssetsImport/ExternalFormats/BVHImport.h"

using namespace BLAengine;

bool AnimationDemo::InitializeEngine(RenderWindow* _renderWindow)
{
    this->m_renderWindow = _renderWindow;
    m_assetManager = new AssetManager();

    m_renderingManager = new RenderingManager(RenderingManager::Game);
    m_debugRenderingManager = new DebugRenderingManager();

    m_editorRenderer = new GL33Renderer();

    m_editorRenderer->InitializeRenderer(this->m_renderWindow, m_renderingManager, m_debugRenderingManager);
    m_editorRenderer->m_assetManager = m_assetManager;

    m_timer = new Time(10);

    m_editorScene = new Scene();

    m_workingScene = new Scene();

    m_workingScene->SetTimeObject(m_timer);

    m_sceneManager = new SceneManager(m_assetManager);

    m_debug = new Debug(m_debugRenderingManager);

    bool terminationRequest = false;

    m_workingScene->Initialize(m_renderingManager);

    // OPENGL CONTEXT UP AND RUNNING
    if (!m_editorRenderer->GetStatus())
    {
        printf("Failed to initiate Context!");
        return false;
    }

    m_assetManager->LoadCookedAssets();

    m_lastHairPlacementTime = m_timer->GetTime();

    return true;
}

void AnimationDemo::UpdateEditor()
{
    GameObject* animationObject = m_workingScene->FindObjectByName("AnimatedObject");

    std::vector<blaVec3> jointPositions;
    std::vector<std::pair<blaVec3, blaVec3>> bones;
    
    if (animationObject != nullptr)
    {
        if (auto animCmp = animationObject->GetComponent<AnimationComponent>())
        {
            if (animCmp->m_animation == nullptr)
            {
                animCmp->m_animation = BVHImport::ImportAnimation("./resources/animations/bvh/01_01.bvh");
            }
            else
            {
                blaF32 animDt = 1.0f / animCmp->m_animation->GetSamplingRate();
                if (m_renderWindow->GetKeyPressed(GLFW_KEY_RIGHT))
                {
                    m_frameIndex += 0.1f;
                }
                if (m_renderWindow->GetKeyPressed(GLFW_KEY_LEFT))
                {
                    m_frameIndex -= 0.1f;
                    m_frameIndex = m_frameIndex < 0.f ? 0.f : m_frameIndex;
                }

                m_frameIndex = m_frameIndex >= animCmp->m_animation->GetFrameCount() ? 0.f : m_frameIndex;
                animCmp->m_animation->QuerySkeletalAnimation(static_cast<int>(m_frameIndex), 0, true, &jointPositions, nullptr, &bones, nullptr);
            }
        }
    }

    blaVec3 offset(0.f, 3.f, 0.f);
    for (size_t i = 0; i < bones.size(); i++)
    {
        m_debug->DrawLine(offset + bones[i].first, offset + bones[i].second);
        //m_debug->DrawLine(offset , offset + jointPositions[i]);
    }

    if (m_editorControls)
    {
        m_editorControls->ControlCamera();
    }
    else
    {
        if (CameraComponent* cameraObject = m_workingScene->GetMainCamera())
        {
            m_editorControls = new AnimationDemoControls(cameraObject->m_parentObject, m_renderWindow);
        }
    }

    m_debug->DrawRay(debugRay, blaVec3(0, 1, 0));
    //m_debug->DrawGrid(1000, 10, blaVec3(0.4));

    ObjectTransform identity;
    m_debug->DrawBasis(&identity,1.f);
    m_debug->Update();
    m_timer->Update();
    m_workingScene->Update();
    m_editorRenderer->Update();

}

void AnimationDemo::TerminateEditor()
{
    //renderWindow->~GLFWRenderWindow();
}

bool AnimationDemo::LoadWorkingScene(std::string filepath)
{
    m_renderingManager->~RenderingManager();
    m_workingScene->~Scene();
    Scene* scenePtr = m_sceneManager->LoadScene(filepath);
    m_workingScene = scenePtr;
    m_workingScene->SetTimeObject(m_timer);
    m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
    m_workingScene->Initialize(m_renderingManager);
    m_editorRenderer->SwitchRenderingManager(m_renderingManager);

    GameObject* light = m_workingScene->CreateObject("DirLight");
    DirectionalLight* dirLight = new DirectionalLight(blaVec3(0, 10, 0));
    light->AddComponent(dirLight);
    ObjectTransform lightT = light->GetTransform();
    lightT.SetPosition(blaVec3(0.f, 20.f, 0.f));
    lightT.SetEulerAngles(-0.66f, 0.f, 0.f);
    light->SetTransform(lightT);

    GameObject* cameraObject = m_workingScene->CreateObject("EditorCamera");
    CameraComponent* cameraComp = new CameraComponent();
    cameraObject->AddComponent(cameraComp);
    ObjectTransform cameraT= cameraObject->GetTransform();
    cameraT.SetPosition(blaVec3(0.f, 20.f, 0.f));
    cameraT.SetEulerAngles(1.2f, 1.2f, 0.f);
    cameraObject->SetTransform(cameraT);

    m_editorRenderer->SetCamera(cameraComp);

    GameObject* animatedObject = m_workingScene->CreateObject("AnimatedObject");
    AnimationComponent* animationCmp = new AnimationComponent();
    animatedObject->AddComponent(animationCmp);
    
    return true;
}

bool AnimationDemo::SaveWorkingScene(std::string filepath)
{
    return m_sceneManager->SaveScene(filepath, m_workingScene);
}

std::vector<string> AnimationDemo::GetSceneObjects()
{
    std::vector<string> objs;
    for (auto go : m_workingScene->GetObjects())
    {
        objs.push_back(go->GetName());
    }
    return objs;
}

void AnimationDemoControls::ControlCamera()
{
    if (m_renderWindow->GetKeyPressed(GLFW_KEY_ESCAPE))
    {
        exit(0);
    }

    ObjectTransform transform = m_cameraObject->GetTransform();

    blaVec3 tangentForce = blaVec3(0);
    blaVec3 movementForce = blaVec3(0);
    float coeff = 0.1f;

    bool hideMouse = false;

    if (m_renderWindow->GetKeyPressed('W'))
        tangentForce.z = -2.f;
    if (m_renderWindow->GetKeyPressed('S'))
        tangentForce.z = 2.f;

    if (m_renderWindow->GetKeyPressed('A'))
        tangentForce.x = -2.f;
    if (m_renderWindow->GetKeyPressed('D'))
        tangentForce.x = 2.f;

    if (m_renderWindow->GetKeyPressed('Q'))
        tangentForce.y = -2.f;
    if (m_renderWindow->GetKeyPressed('E'))
        tangentForce.y = 2.f;

    if (m_renderWindow->GetMousePressed(2))
    {
        double x, y;
        m_renderWindow->GetMouse(x, y);
        tangentForce = blaVec3(0, 0, 0);

        if (x - m_prevMouse.x > 0)
        {
            tangentForce.x = -1.f;
        }
        else if (x - m_prevMouse.x < 0)
        {
            tangentForce.x = 1.f;
        }
        if (y - m_prevMouse.y > 0)
        {
            tangentForce.y = 1.f;
        }
        else if (y - m_prevMouse.y < 0)
        {
            tangentForce.y = -1.f;
        }

        m_prevMouse = glm::vec2(x, y);

        hideMouse = true;
    }

    blaVec3 cameraForce = transform.LocalDirectionToWorld(tangentForce);
    cameraForce *= coeff;
    transform.SetPosition(transform.GetPosition() + cameraForce);

    if (m_renderWindow->GetMousePressed(1))
    {
        double x, y;
        m_renderWindow->GetMouse(x, y);
        glm::vec2 curMouse = glm::vec2(x, y);

        blaVec3 deltaRotation = blaVec3(0);

        if (x - m_prevMouse.x > 0)
        {
            deltaRotation.y = -1.f;
        }
        else if (x - m_prevMouse.x < 0)
        {
            deltaRotation.y = 1.f;
        }
        if (y - m_prevMouse.y > 0)
        {
            deltaRotation.x = -1.f;
        }
        else if (y - m_prevMouse.y < 0)
        {
            deltaRotation.x = 1.f;
        }

        m_prevMouse = curMouse;

        m_cameraRotation += 0.033f * deltaRotation;

        transform.SetEulerAngles(m_cameraRotation.x, m_cameraRotation.y, m_cameraRotation.z);

        hideMouse = true;
    }

    m_renderWindow->SetMouseCursorVisibility(!hideMouse);

    m_cameraObject->SetTransform(transform);

    m_cameraObject->Update();
}
