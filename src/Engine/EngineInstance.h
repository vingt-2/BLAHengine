#include <Common/StdInclude.h>
#include <Common/BLASingleton.h>

namespace BLAengine
{
    class Console;
    class GL33Renderer;
    class AssetManager;
    class SceneManager;
    class DebugDraw;
    class Scene;
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
            m_isTerminationRequested(false)
        {};

        ~EngineInstance() { TerminateEngine(); };

        Scene* GetWorkingScene() const { return m_workingScene; }

        static blaU32 LoopEngine();
    
    protected:

        virtual bool InitializeEngine(RenderWindow* renderWindow);
        virtual void PreEngineUpdate();
        virtual void EngineUpdate();
        virtual void PostEngineUpdate();
        virtual void TerminateEngine();

        bool ShouldTerminate() const { return m_isTerminationRequested; };

        bool SaveWorkingScene(std::string filePath);

        const Timer* GetTimer() const { return m_timer; }

        virtual bool LoadNewScene();

        virtual bool LoadWorkingScene(std::string filePath);

        void SetupDirLightAndCamera();

        // Required Engine Modules
        Console*      m_console;
        GL33Renderer* m_renderer;
        AssetManager* m_assetManager;
        SceneManager* m_sceneManager;
        DebugDraw* m_debug;
        Scene* m_workingScene;
        Scene* m_editorScene;
        RenderWindow* m_renderWindow;
        Timer* m_timer;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;
        InputManager* m_inputManager;
        BlaGuiManager* m_guiManager;

        bool m_isFullScreen;
        bool m_isTerminationRequested;
    };
};