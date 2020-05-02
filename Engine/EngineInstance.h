#pragma once

#include "StdInclude.h"
#include "BLASingleton.h"

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

    class BLACORE_API EngineInstance
    {
        BLA_DECLARE_SINGLETON(EngineInstance)

    public:
        EngineInstance(bool external, bool isFullscreen) :
            m_isFullScreen(isFullscreen),
            m_isTerminationRequested(false),
            m_isCapturedMouse(false)
        {};

        ~EngineInstance() { TerminateEngine(); };

        Scene* GetWorkingScene() const { return m_scene; }

        static blaU32 LoopEngine();

        void RequestShutdown();

        void SetTargetFPS(int fps) { m_targetFPS = fps; }
        blaU32 GetTargetFPS() { return m_targetFPS; }

        GL33Renderer& GetRenderer() { return *m_renderer; }

        int GetEngineVersion() const { return m_engineVersion; }

    protected:
        blaS32 m_targetFPS = 144;

        virtual bool InitializeEngine(RenderWindow* renderWindow);
        virtual void PreEngineUpdate();
        virtual void EngineUpdate();
        virtual void PostEngineUpdate();
        virtual void TerminateEngine();

		virtual void InitializeComponentLibrariesManager();

        bool ShouldTerminate() const { return m_isTerminationRequested; };

        bool SaveWorkingScene(blaString filePath);

        const Timer* GetTimer() const { return m_timer; }

        virtual bool LoadNewScene();

        virtual bool LoadWorkingScene(blaString filePath);

        void SetupDirLightAndCamera();

        void ToggleCaptureMouse();

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