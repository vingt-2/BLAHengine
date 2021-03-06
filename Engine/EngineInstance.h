// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "StdInclude.h"
#include "BLASingleton.h"
#include "System.h"

namespace BLA
{
    namespace Core
    {
        class Scene;
        class ComponentLibrariesManager;
    }
    class Console;
    class AssetManager;
    class SceneManager;
    class DebugDraw;
    class RenderWindow;
    class Timer;
    class InputManager;
    class DevGuiManager;
    class Renderer;

    class EngineInstance
    {
        BLA_DECLARE_EXPORTED_SINGLETON(EngineInstance)

    public:
        EngineInstance(bool external, bool isFullscreen) :
            m_isFullScreen(isFullscreen),
            m_isTerminationRequested(false),
            m_isCapturedMouse(false)
        {};

        BLACORE_API ~EngineInstance() { TerminateEngine(); };

        BLACORE_API Core::Scene* GetWorkingScene() const { return m_scene; }

        BLACORE_API static blaU32 LoopEngine();

        BLACORE_API void RequestShutdown();

        BLACORE_API void SetTargetFPS(int fps) { m_targetFPS = fps; }
        BLACORE_API blaU32 GetTargetFPS() { return m_targetFPS; }

        BLACORE_API int GetEngineVersion() const { return m_engineVersion; }

    protected:
        blaS32 m_targetFPS = 144;

        BLACORE_API virtual bool InitializeEngine(RenderWindow* renderWindow);
        BLACORE_API virtual void PreEngineUpdate();
        BLACORE_API virtual void EngineUpdate();
        BLACORE_API virtual void PostEngineUpdate();
        BLACORE_API virtual void TerminateEngine();
        
        BLACORE_API virtual void InitializeComponentLibrariesManager();

        BLACORE_API bool ShouldTerminate() const { return m_isTerminationRequested; };

        BLACORE_API bool SaveWorkingScene(blaString filePath);

        BLACORE_API const Timer* GetTimer() const { return m_timer; }

        BLACORE_API virtual bool LoadNewScene();

        BLACORE_API virtual bool LoadWorkingScene(blaString filePath);

        BLACORE_API void SetupDirLightAndCamera();

        BLACORE_API void ToggleCaptureMouse();

        // Required Engine Modules
        Console*      m_console;

        Renderer*     m_renderer;
   
        AssetManager* m_assetManager;
        Core::ComponentLibrariesManager* m_componentLibrariesManager;
        SceneManager* m_sceneManager;
        DebugDraw* m_debug;
        Core::Scene* m_scene;
        RenderWindow* m_renderWindow;
        Timer* m_timer;
        InputManager* m_inputManager;
        DevGuiManager* m_guiManager;

        bool m_isFullScreen;
        bool m_isTerminationRequested;
        bool m_isCapturedMouse;
        int m_engineVersion;
    };
};