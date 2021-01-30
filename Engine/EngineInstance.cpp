// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include <thread>

#include "System.h"
#include "Maths/Maths.h"

#include "Core/Timer.h"
#include "Core/DebugDraw.h"
#include "Core/GameComponent.h"
#include "Assets/SceneManager.h"
#include "System/Console.h"
#include "System/InputManager.h"
#include "System/RenderWindow.h"
#include "System/Time.h"
#include "Gui/DevGuiManager.h"
#include "System/ControllerInputs/Dualshock4.h"
#include "Core/ComponentLibraries.h"
#include "Core/ComponentSystems.h"
#include "EngineInstance.h"
#include "Rendering/Renderer.h"
#include "Assets/AssetsManager.h"

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(EngineInstance)

blaU32 EngineInstance::LoopEngine()
{
    EngineInstance* engineInstance = EngineInstance::GetSingletonInstance();

    GLFWRenderWindow::InitGLFW();

    RenderWindow* renderWindow = new GLFWVulkanRenderWindow();

    renderWindow->CreateRenderWindow("BLAengine Editor", 1280, 720, false);

    engineInstance->InitializeEngine(renderWindow);

    engineInstance->LoadWorkingScene("./sponza.blaScene");

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
    m_engineVersion = BLA_ENGINE_VERSION;

    m_console = Console::GetSingletonInstance();
    if (!m_console)
        Console::AssignAndReturnSingletonInstance(new Console());

    m_inputManager = InputManager::AssignAndReturnSingletonInstance(new InputManager());

    this->m_renderWindow = renderWindow;

    m_assetManager = AssetManager::AssignAndReturnSingletonInstance(new AssetManager());

    InitializeComponentLibrariesManager();

    m_renderer = new Renderer(static_cast<GLFWRenderWindow*>(m_renderWindow));

    m_timer = Timer::AssignAndReturnSingletonInstance(new Timer(10));

    m_scene = Core::Scene::AssignAndReturnSingletonInstance(new Core::Scene());

    m_sceneManager = new SceneManager();

    m_debug = new DebugDraw();

    DebugDraw::AssignSingletonInstance(m_debug);

    m_componentLibrariesManager->LoadLibraries();
    Core::ComponentSystemsRegistry::GetSingletonInstance()->FinalizeLoad();

    m_scene->Initialize();

    // Finally, load our assets in memory...
    m_assetManager->LoadCookedAssets();

    return true;
}

void EngineInstance::PreEngineUpdate()
{
    m_timer->Update(System::GetSystemTime());
}

void EngineInstance::EngineUpdate()
{
    m_scene->Update();
}

void EngineInstance::PostEngineUpdate()
{
    m_debug->Update();

    m_renderer->Update();

   // m_guiManager->Update(false);

    // Inputs should be the second to last thing to update !
    m_inputManager->Update();

    /*if (!m_isCapturedMouse)
    {
        m_inputManager->SetMouseLock(m_guiManager->IsMouseOverGui());
        m_inputManager->SetKeyboardLock(m_guiManager->IsMouseOverGui());
    }*/

    // Final update of the frame
    m_renderWindow->UpdateWindowAndBuffers();
}

void EngineInstance::TerminateEngine()
{    
    delete m_inputManager;
    delete m_debug;
    delete m_scene;
    delete m_timer;
    delete m_renderer;
    delete m_sceneManager;
    delete m_assetManager;
    delete m_guiManager;
    delete m_renderWindow;
    delete Core::GameComponentRegistry::GetSingletonInstance();
    delete m_componentLibrariesManager;

    GLFWRenderWindow::ShutdownGLFW();
}

void EngineInstance::InitializeComponentLibrariesManager()
{
    m_componentLibrariesManager = new Core::ComponentLibrariesManager();
}

bool EngineInstance::LoadNewScene()
{
    Core::Scene::GetSingletonInstance()->Clear();

    SetupDirLightAndCamera();

    m_renderWindow->SetWindowTitle("New Scene");

    return true;
}

bool EngineInstance::LoadWorkingScene(blaString filepath)
{
    m_sceneManager->LoadScene(filepath);

    // SetupDirLightAndCamera();
    // m_renderer->SetCamera(m_scene->GetMainCamera());

    FileEntry sceneFileEntry = ParseFilePath(filepath);

    m_renderWindow->SetWindowTitle(sceneFileEntry.m_name);

    return true;
}

bool EngineInstance::SaveWorkingScene(blaString filepath)
{
    return m_sceneManager->SaveScene(filepath, m_scene);
}

void EngineInstance::SetupDirLightAndCamera()
{
    /*GameObject light = m_scene->CreateObject(BlaStringId("DirLight"));
    light.CreateComponent(BlaStringId("DirectionalLightComponent"));*/

    Core::GameObject cameraObject = m_scene->CreateObject(BlaStringId("EditorCamera"));
    // CameraComponent* cameraComp = cameraObject.CreateComponent<CameraComponent>();

    // m_renderer->SetCamera(cameraComp);
}

void EngineInstance::ToggleCaptureMouse()
{
    m_isCapturedMouse = !m_isCapturedMouse;
    m_renderWindow->SetMouseCursorVisibility(!m_isCapturedMouse);
}

DefineConsoleCommand(void, exit)
{
    EngineInstance::GetSingletonInstance()->RequestShutdown();
}

DefineConsoleCommand(void, shutdown)
{
    EngineInstance::GetSingletonInstance()->RequestShutdown();
}

DefineConsoleCommand(void, SetFPS, int fps)
{
    EngineInstance::GetSingletonInstance()->SetTargetFPS(fps);
}