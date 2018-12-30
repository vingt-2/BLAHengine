#include "../Common/System.h"
#include "../Common/Maths/Maths.h"
#include "./Renderer/GL33Renderer.h"
#include "./Game/RenderingManager.h"
#include "./Game\Debug.h"
#include "./Assets/SceneManager.h"

#include "EngineInstance.h"

using namespace BLAengine;

BLA_DECLARE_SINGLETON(Debug);

bool EngineInstance::InitializeEngine(RenderWindow* renderWindow)
{
    this->m_renderWindow = renderWindow;
    m_assetManager = new AssetManager();

    m_renderingManager = new RenderingManager(RenderingManager::Game);
    m_debugRenderingManager = new DebugRenderingManager();

    m_renderer = new GL33Renderer();

    m_renderer->InitializeRenderer(this->m_renderWindow, m_renderingManager, m_debugRenderingManager);
    m_renderer->m_assetManager = m_assetManager;

    m_timer = new Timer(10);

    m_editorScene = new Scene();

    m_workingScene = new Scene();

    m_workingScene->SetTimeObject(m_timer);

    m_sceneManager = new SceneManager(m_assetManager);

    m_debug = new Debug(m_debugRenderingManager);

	BLA_ASSIGN_SINGLETON(Debug, m_debug);

    m_workingScene->Initialize(m_renderingManager);

    // Is the renderer and its window up and running ?
    if (!m_renderer->GetStatus())
    {
        printf("Failed to initiate renderer and / or render window... Terminating.\n");
        return false;
    }

    // Finally, load our assets in memory...
    m_assetManager->LoadCookedAssets();

    return true;
}

void EngineInstance::PreEngineUpdate()
{
    m_timer->Update();
    //TODO: Build an Input Manager and scan inputs here...
}

void EngineInstance::EngineUpdate()
{
    if (m_renderWindow->GetKeyPressed(GLFW_KEY_ESCAPE))
    {
        m_isTerminationRequested = true;
    }
    
    m_workingScene->Update();
}

void EngineInstance::PostEngineUpdate()
{
	m_debug->Update();
    m_renderer->Update();
}

void EngineInstance::TerminateEngine()
{
    //TODO: Clean the render window ...
    //m_renderWindow->~RenderWindow();
}

bool BLAengine::EngineInstance::LoadWorkingScene(std::string filepath)
{
    m_renderingManager->~RenderingManager();
    m_workingScene->~Scene();
    Scene* scenePtr = m_sceneManager->LoadScene(filepath);
    m_workingScene = scenePtr;
    m_workingScene->SetTimeObject(m_timer);
    m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
    m_workingScene->Initialize(m_renderingManager);
    m_renderer->SwitchRenderingManager(m_renderingManager);

    SetupDirLightAndCamera();

    return true;
}

bool EngineInstance::SaveWorkingScene(std::string filepath)
{
    return m_sceneManager->SaveScene(filepath, m_workingScene);
}

void EngineInstance::SetupDirLightAndCamera()
{
    GameObject* light = m_workingScene->CreateObject("DirLight");
    DirectionalLight* dirLight = BLA_CREATE_COMPONENT(DirectionalLight, light);
    ObjectTransform lightT = light->GetTransform();
    lightT.SetPosition(blaVec3(0.f, 20.f, 0.f));
    lightT.SetEulerAngles(-0.93f, 0.f, 0.f);
    light->SetTransform(lightT);

    GameObject* cameraObject = m_workingScene->CreateObject("EditorCamera");
    CameraComponent* cameraComp = BLA_CREATE_COMPONENT(CameraComponent, cameraObject);

	ObjectTransform  cameraStartTransform;

	cameraStartTransform.SetPosition(blaVec3(0.f, 2.f, 2.f));

	cameraObject->SetTransform(cameraStartTransform);

    m_renderer->SetCamera(cameraComp);
}