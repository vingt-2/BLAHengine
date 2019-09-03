#include <thread>

#include <Common/System.h>
#include <Common/Maths/Maths.h>
#include <Engine/Renderer/GL33Renderer.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/RenderingManager.h>
#include <Engine/Core/DebugDraw.h>
#include <Engine/Core/GameComponent.h>
#include <Engine/Assets/SceneManager.h>
#include <Engine/System/Console.h>
#include <Engine/System/InputManager.h>
#include <Engine/System/RenderWindow.h>
#include <Engine/Gui/GuiManager.h>
#include <Engine/System/ControllerInputs/Dualshock4.h>

#include "EngineInstance.h"

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(EngineInstance)

blaU32 EngineInstance::LoopEngine()
{
    EngineInstance* engineInstance = EngineInstance::GetSingletonInstance();

    RenderWindow* renderWindow = new GLFWRenderWindow();

    renderWindow->CreateRenderWindow("BLAengine Editor", 1280, 720, false);

    engineInstance->InitializeEngine(renderWindow);

    Dualshock4 controller;

    while (!engineInstance->ShouldTerminate())
    {
        auto frameStartTime = std::chrono::steady_clock::now();

        // We should have a controller manager that does that and looks for more periodically
        if (!controller.GetDS4Found())
        {
            controller.Setup();
        }

        if (controller.GetDS4Found())
        {
            controller.Update();
        }

        engineInstance->PreEngineUpdate();
        engineInstance->EngineUpdate();
        engineInstance->PostEngineUpdate();

        auto frameEndTime = std::chrono::steady_clock::now();
        int microSecondsFrameTime = (int)(1000000.f / engineInstance->GetTargetFPS());
        auto waitDuration = std::chrono::microseconds(microSecondsFrameTime) - (frameEndTime - frameStartTime);

        std::this_thread::sleep_for(waitDuration);
    }

    // Call terminates engine
    engineInstance->TerminateEngine();

    return 0;
}

void EngineInstance::RequestShutdown()
{
    m_isTerminationRequested = true;;
}

bool EngineInstance::InitializeEngine(RenderWindow* renderWindow)
{
    m_console = Console::GetSingletonInstance();
    if (!m_console)
        Console::AssignAndReturnSingletonInstance(new Console());

    m_inputManager = InputManager::AssignAndReturnSingletonInstance(new InputManager());

    this->m_renderWindow = renderWindow;

    m_assetManager = AssetManager::AssignAndReturnSingletonInstance(new AssetManager());

    m_renderingManager = new RenderingManager(RenderingManager::Game);
    m_debugRenderingManager = new DebugRenderingManager();

    m_renderer = new GL33Renderer();

    m_renderer->InitializeRenderer(this->m_renderWindow, m_renderingManager, m_debugRenderingManager);
    m_renderer->m_assetManager = m_assetManager;

    m_timer = Timer::AssignAndReturnSingletonInstance(new Timer(10));

    m_editorScene = new Scene();

    m_workingScene = new Scene();

    m_workingScene->SetTimeObject(m_timer);

    m_sceneManager = new SceneManager(m_assetManager);

    m_debug = new DebugDraw(m_debugRenderingManager);

    DebugDraw::AssignSingletonInstance(m_debug);

    m_workingScene->Initialize(m_renderingManager);

    m_guiManager = BlaGuiManager::AssignAndReturnSingletonInstance(new BlaGuiManager((dynamic_cast<GLFWRenderWindow*>(m_renderWindow))->GetWindowPointer()));

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
    delete m_inputManager;
    delete m_debug;
    delete m_workingScene;
    delete m_timer;
    delete m_renderingManager;
    delete m_debugRenderingManager;
    delete m_renderer;
    delete m_sceneManager;
    delete m_assetManager;
    delete m_guiManager;
    delete m_renderWindow;
    delete GameComponentManager::GetSingletonInstance();
}

bool EngineInstance::LoadNewScene()
{
    delete m_renderingManager;
    delete m_workingScene;

    m_workingScene = new Scene();

    m_workingScene->SetTimeObject(m_timer);

    m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
    m_workingScene->Initialize(m_renderingManager);
    m_renderer->SwitchRenderingManager(m_renderingManager);

    SetupDirLightAndCamera();

    m_renderWindow->SetWindowTitle("BLA Editor - New Scene");

    return true;
}

bool EngineInstance::LoadWorkingScene(blaString filepath)
{
    delete m_renderingManager;
    delete m_workingScene;

    Scene* scenePtr = m_sceneManager->LoadScene(filepath);

    m_workingScene = scenePtr;
    m_workingScene->SetTimeObject(m_timer);

    m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
    m_workingScene->Initialize(m_renderingManager);
    m_renderer->SwitchRenderingManager(m_renderingManager);

    SetupDirLightAndCamera();

    FileEntry sceneFileEntry = ParseFilePath(filepath);

    m_renderWindow->SetWindowTitle("BLA Editor - " + sceneFileEntry.m_name);

    return true;
}

bool EngineInstance::SaveWorkingScene(blaString filepath)
{
    return m_sceneManager->SaveScene(filepath, m_workingScene);
}

void EngineInstance::SetupDirLightAndCamera()
{
    GameObjectReference light = m_workingScene->CreateObject("DirLight");
    DirectionalLightComponent* dirLight = BLA_CREATE_COMPONENT(DirectionalLightComponent, light);
    ObjectTransform lightT = light->GetTransform();
    lightT.SetPosition(blaVec3(0.f, 20.f, 0.f));
    lightT.SetEulerAngles(-0.93f, 0.f, 0.f);
    light->SetTransform(lightT);

    GameObjectReference cameraObject = m_workingScene->CreateObject("EditorCamera");
    CameraComponent* cameraComp = BLA_CREATE_COMPONENT(CameraComponent, cameraObject);

    ObjectTransform  cameraStartTransform;

    cameraStartTransform.SetPosition(blaVec3(0.f, 2.f, 15.f));

    cameraObject->SetTransform(cameraStartTransform);

    m_renderer->SetCamera(cameraComp);
}

BLA_CONSOLE_COMMAND(int, exit)
{
	EngineInstance::GetSingletonInstance()->RequestShutdown();
	return 0;
}

BLA_CONSOLE_COMMAND(int, shutdown)
{
	EngineInstance::GetSingletonInstance()->RequestShutdown();
	return 0;
}

BLA_CONSOLE_COMMAND(int, SetFPS, int fps) 
{
	EngineInstance::GetSingletonInstance()->SetTargetFPS(fps);
	return 0;
}