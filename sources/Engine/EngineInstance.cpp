#include <Common/System.h>
#include <Common/Maths/Maths.h>
#include <Engine/Renderer/GL33Renderer.h>
#include <Engine/Game/RenderingManager.h>
#include <Engine/Game/Debug.h>
#include <Engine/Assets/SceneManager.h>
#include <Engine/System/InputManager.h>
#include <Engine/System/RenderWindow.h>
#include <Engine/Gui/GuiManager.h>

#include "EngineInstance.h"

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(EngineInstance)

void DragAndDropHandler(DragAndDropPayloadDontStore* dragAndDropInput)
{
    for (auto path : *dragAndDropInput)
    {
        std::cout << path << "\n";
    }
}

bool EngineInstance::InitializeEngine(RenderWindow* renderWindow)
{
    m_inputManager = InputManager::AssignAndReturnSingletonInstance(new InputManager());

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

    Debug::AssignSingletonInstance(m_debug);

    m_workingScene->Initialize(m_renderingManager);

    m_guiManager = new BlaGuiManager((dynamic_cast<GLFWRenderWindow*>(m_renderWindow))->GetWindowPointer());

    // Is the renderer and its window up and running ?
    if (!m_renderer->GetStatus())
    {
        printf("Failed to initiate renderer and / or render window... Terminating.\n");
        return false;
    }

    // Finally, load our assets in memory...
    m_assetManager->LoadCookedAssets();

    m_renderWindow->SetDragAndDropCallback((DragAndDropCallback) DragAndDropHandler);

    return true;
}

void EngineInstance::PreEngineUpdate()
{
    m_timer->Update(glfwGetTime());
    //TODO: Build an Input Manager and scan inputs here...
}

void EngineInstance::EngineUpdate()
{    
    m_workingScene->Update();
}

void EngineInstance::PostEngineUpdate()
{
    m_debug->Update();
    m_renderer->Update();

    m_guiManager->Update();

    // Inputs should be the second to last thing to update !
    m_inputManager->Update();

    m_inputManager->m_lockMouse = m_guiManager->IsMouseOverGui();
    // Final update of the frame
    m_renderWindow->UpdateWindowAndBuffers();
}

void EngineInstance::TerminateEngine()
{
    //TODO: Clean the render window ...
    //m_renderWindow->~RenderWindow();
    delete m_guiManager;
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