#include "../Common/StdInclude.h"

namespace BLAengine
{
    class GL33Renderer;
    class AssetManager;
    class SceneManager;
    class Debug;
    class Scene;
    class Scene;
    class RenderWindow;
    class Time;
    class RenderingManager;
    class DebugRenderingManager;

    class BLACORE_API EngineInstance
    {
    public:
        EngineInstance(bool external, bool isFullscreen) :
            m_isFullScreen(isFullscreen),
            m_isTerminationRequested(false)
        {};

        ~EngineInstance() { TerminateEngine(); };

        bool InitializeEngine(RenderWindow* renderWindow);

        virtual void PreEngineUpdate();

        virtual void EngineUpdate();

        virtual void PostEngineUpdate();

        virtual void TerminateEngine();

        virtual bool LoadWorkingScene(std::string filePath);

        bool ShouldTerminate() const { return m_isTerminationRequested; };

        bool SaveWorkingScene(std::string filePath);

        Debug* GetDebug() const { return m_debug; }

    protected:

        void SetupDirLightAndCamera();

        // Required Engine Modules
        GL33Renderer* m_renderer;
        AssetManager* m_assetManager;
        SceneManager* m_sceneManager;
        Debug* m_debug;
        Scene* m_workingScene;
        Scene* m_editorScene;
        RenderWindow* m_renderWindow;
        Time* m_timer;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;

        bool m_isFullScreen;
        bool m_isTerminationRequested;
    };
};