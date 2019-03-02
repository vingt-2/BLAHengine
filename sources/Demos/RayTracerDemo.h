#include <Common/StdInclude.h>
#include <Common/System.h>
#include <Common/Maths/Maths.h>
#include <Engine/Renderer/GL33Renderer.h>
#include <Engine/Game/RenderingManager.h>
#include <Engine/Debug/Debug.h>
#include <Engine/Assets/SceneManager.h>

namespace BLAengine
{
    class BLACORE_API Raytracer
    {
    public:
        Raytracer(bool external, bool isFullscreen) :
            m_isFullScreen(isFullscreen),
            m_isTerminationRequested(false),
            m_pbr_render_requested(false),
            debugRay(Ray(blaVec3(0), blaVec3(0), 1))
        {};

        ~Raytracer() { TerminateEditor(); };

        bool InitializeEngine();

        void UpdateEditor();

        void TerminateEditor();

        bool ShouldTerminate() { return m_isTerminationRequested; };

        bool LoadWorkingScene(std::string filePath);

        bool SaveWorkingScene(std::string filePath);

    private:

        // Required Engine Modules
        GL33Renderer* m_editorRenderer;
        AssetManager* m_assetManager;
        SceneManager* m_sceneManager;
        Debug* m_debug;
        Scene* m_workingScene;
        Scene* m_editorScene;
        RenderWindow* m_renderWindow;
        Timer* m_timer;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;

        //States
        bool m_pbr_render_requested;
        Ray debugRay;
        bool m_isFullScreen;
        bool m_isTerminationRequested;
    };
};