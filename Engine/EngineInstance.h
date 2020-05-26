// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "StdInclude.h"
#include "BLASingleton.h"
#include "System.h"

namespace BLA
{
    class Console;
    class GL33Renderer;
    class AssetManager;
	class ComponentLibrariesManager;
    class SceneManager;
    class DebugDraw;
    class Scene;
    class RenderWindow;
    class Timer;
    class RenderingManager;
    class DebugRenderingManager;
    class InputManager;
    class BlaGuiManager;

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

        BLACORE_API Scene* GetWorkingScene() const { return m_scene; }

        BLACORE_API static blaU32 LoopEngine();

        BLACORE_API void RequestShutdown();

        BLACORE_API void SetTargetFPS(int fps) { m_targetFPS = fps; }
        BLACORE_API blaU32 GetTargetFPS() { return m_targetFPS; }

        BLACORE_API GL33Renderer& GetRenderer() { return *m_renderer; }

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
        GL33Renderer* m_renderer;
        AssetManager* m_assetManager;
		ComponentLibrariesManager* m_componentLibrariesManager;
        SceneManager* m_sceneManager;
        DebugDraw* m_debug;
        Scene* m_scene;
        RenderWindow* m_renderWindow;
        Timer* m_timer;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;
        InputManager* m_inputManager;
        BlaGuiManager* m_guiManager;

        bool m_isFullScreen;
        bool m_isTerminationRequested;
        bool m_isCapturedMouse;
        int m_engineVersion;
    };
};