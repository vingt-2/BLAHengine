#include "EditorSession.h"
#include "../Game/PBRendering/PBRRenderer.h"

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

    return true;
}

void EditorSession::UpdateEditor()
{
    if (m_renderWindow->GetMousePressed(0))
    {
        Ray screenRay = m_editorRenderer->ScreenToRay();

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

    if (m_editorControls)
    {
        m_editorControls->ControlCamera();
    }
    else
    {
        if (CameraComponent* cameraObject = m_workingScene->GetMainCamera())
        {
            m_editorControls = new EditorControls(cameraObject->GetParentObject(), m_renderWindow);
        }
    }

    m_debug->DrawRay(debugRay, blaVec3(0, 1, 0));
    //m_debug->DrawGrid(1000, 10, blaVec3(0.4));
    m_debug->Update();
    m_timer->Update();
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

void BLAengine::EditorControls::ControlCamera()
{
    if (m_renderWindow->GetKeyPressed(GLFW_KEY_ESCAPE))
    {
        exit(0);
    }
    
    ObjectTransform transform = m_cameraObject->GetTransform();

    blaVec3 tangentForce = blaVec3(0);
    blaVec3 movementForce = blaVec3(0);
    float coeff = 0.1f;


    if (m_renderWindow->GetKeyPressed('W'))
        tangentForce.z = 2.f;
    if (m_renderWindow->GetKeyPressed('S'))
        tangentForce.z = -2.f;

    if (m_renderWindow->GetKeyPressed('A'))
        tangentForce.x = 2.f;
    if (m_renderWindow->GetKeyPressed('D'))
        tangentForce.x = -2.f;

    if (m_renderWindow->GetKeyPressed('Q'))
        tangentForce.y = 2.f;
    if (m_renderWindow->GetKeyPressed('E'))
        tangentForce.y = -2.f;

    if (m_renderWindow->GetMousePressed(2))
    {
        double x, y;
        m_renderWindow->GetMouse(x, y);
        tangentForce = blaVec3(0, 0, 0);

        if (x - m_prevMouse.x > 0)
        {
            tangentForce.x = 1.f;
        }
        else if (x - m_prevMouse.x < 0)
        {
            tangentForce.x = -1.f;
        }
        if (y - m_prevMouse.y > 0)
        {
            tangentForce.y = 1.f;
        }
        else if (y - m_prevMouse.y < 0)
        {
            tangentForce.y = -1.f;
        }

        m_prevMouse = glm::vec2(x,y);
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
            deltaRotation.y = 1.f;
        }
        else if (x - m_prevMouse.x < 0)
        {
            deltaRotation.y = -1.f;
        }
        if (y - m_prevMouse.y > 0)
        {
            deltaRotation.x = 1.f;
        }
        else if (y - m_prevMouse.y < 0)
        {
            deltaRotation.x = -1.f;
        }

        m_prevMouse = curMouse;

        m_cameraRotation += 0.05f * deltaRotation;
        
        transform.SetEulerAngles(m_cameraRotation.x, m_cameraRotation.y, m_cameraRotation.z);
    }

    m_cameraObject->SetTransform(transform);

    m_cameraObject->Update();
}
