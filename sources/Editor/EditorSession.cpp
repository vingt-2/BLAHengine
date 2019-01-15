#include "EditorSession.h"
#include <Engine/Game/PBRendering/PBRRenderer.h>
#include <Engine/System/InputManager.h>

using namespace BLAengine;

bool EditorSession::InitializeEngine(RenderWindow* _renderWindow)
{
    this->m_renderWindow = _renderWindow;
    m_assetManager = new AssetManager();
    
    m_renderingManager = new RenderingManager(RenderingManager::Game);
    m_debugRenderingManager = new DebugRenderingManager();

    m_editorRenderer = new GL33Renderer();

    m_editorRenderer->InitializeRenderer(this->m_renderWindow, m_renderingManager, m_debugRenderingManager);
    m_editorRenderer->m_assetManager = m_assetManager;

    m_timer = new Timer(10);

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

    return true;
}

void EditorSession::UpdateEditor()
{
    auto inputs = InputManager::GetSingletonInstance();

    if (inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_LEFT).IsDown())
    {
        Ray screenRay = m_editorRenderer->ScreenToRay(m_renderWindow->GetMousePointerScreenSpaceCoordinates());

        if (GameObject* object = m_workingScene->PickGameObjectInScene(screenRay).first)
        {
            ColliderComponent::RayCollision cp = object->GetComponent<ColliderComponent>()->CollideWithRay(screenRay);
            if (RigidBodyComponent* rb = object->GetComponent<RigidBodyComponent>())
            {
                rb->PushForceWorld(cp.m_colPositionW, cp.m_colNormalW);
            }
            else
            {
                BLA_CREATE_COMPONENT(RigidBodyComponent, object);
            }
            debugRay = Ray(cp.m_colPositionW, cp.m_colNormalW, 10000);
        }
    }

    if (GameObject* dirlightObj = m_workingScene->FindObjectByName("DirLight"))
    {
        if (DirectionalLight* dirLight = dirlightObj->GetComponent<DirectionalLight>())
        {
            ObjectTransform lightT = dirLight->GetObjectTransform();
            lightT.SetPosition(blaVec3(0.f, 50.f, 0.f));
            blaVec3 rotationInEuler = lightT.GetEulerAngles();
            //cout << rotationInEuler.y;
            lightT.SetEulerAngles(0.5f*3.14f, 0.f ,0.f);
            dirlightObj->SetTransform(lightT);
        }
    }

    m_debug->DrawRay(debugRay, blaVec3(0, 1, 0));
    //m_debug->DrawGrid(1000, 10, blaVec3(0.4));
    m_debug->Update();
    m_timer->Update(glfwGetTime());
    m_workingScene->Update();
    m_editorRenderer->Update();

}

void EditorSession::TerminateEditor()
{
    //renderWindow->~GLFWRenderWindow();
}

bool BLAengine::EditorSession::LoadWorkingScene(std::string filepath)
{
    m_renderingManager->~RenderingManager();
    m_workingScene->~Scene();
    Scene* scenePtr = m_sceneManager->LoadScene(filepath);
    m_workingScene = scenePtr;
    m_workingScene->SetTimeObject(m_timer);
    m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
    m_workingScene->Initialize(m_renderingManager);
    m_editorRenderer->SwitchRenderingManager(m_renderingManager);

    GameObject* lightObject = m_workingScene->CreateObject("DirLight");
    DirectionalLight* dirLight = BLA_CREATE_COMPONENT(DirectionalLight, lightObject);
    ObjectTransform lightT = lightObject->GetTransform();
    lightT.SetPosition(blaVec3(0.f, 20.f, 0.f));
    lightT.SetEulerAngles(0.f, 1.2f, 0.f);
    lightObject->SetTransform(lightT);

    GameObject* cameraObject = m_workingScene->CreateObject("EditorCamera");
    CameraComponent* cameraComp = BLA_CREATE_COMPONENT(CameraComponent, cameraObject);

    m_editorRenderer->SetCamera(cameraComp);

    return true;
}

bool BLAengine::EditorSession::SaveWorkingScene(std::string filepath)
{
    return m_sceneManager->SaveScene(filepath, m_workingScene);
}

std::vector<string> BLAengine::EditorSession::GetSceneObjects()
{
    std::vector<string> objs;
    for (auto go : m_workingScene->GetObjects())
    {
        objs.push_back(go->GetName());
    }
    return objs;
}
