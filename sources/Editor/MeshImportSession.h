#include <Common/StdInclude.h>
#include <Common/System.h>
#include <Common/Maths/Maths.h>
#include <Engine/Renderer/GL33Renderer.h>
#include <Engine/Game/RenderingManager.h>
#include <Engine/Debug/Debug.h>
#include <Engine/Assets/SceneManager.h>

namespace BLAengine
{
    class BLACORE_API MeshEditorControls
    {
    public:

        glm::vec2 m_prevMouse;
        blaVec3 m_cameraRotation;
        blaVec3 m_lightRotation;

        GameObject* m_cameraObject;
        GameObject* m_lightObj;
        RenderWindow* m_renderWindow;

        MeshEditorControls(GameObject* cameraObject, GameObject* lightObj, RenderWindow* renderWindow) :
            m_prevMouse(glm::vec2(0)),
            m_cameraRotation(blaVec3(0)),
            m_lightRotation(blaVec3(0)),
            m_cameraObject(cameraObject),
            m_renderWindow(renderWindow),
            m_lightObj(lightObj)
        {};


        ~MeshEditorControls() {};

        void ControlCamera();

    };

    class BLACORE_API MeshImportSession
    {
    public:
        MeshImportSession(bool external, bool isFullscreen) :
            m_isFullScreen(isFullscreen),
            m_isTerminationRequested(false)
        {};

        ~MeshImportSession() { TerminateEditor(); };

        bool InitializeEngine(RenderWindow* renderWindow);

        void UpdateEditor();

        void TerminateEditor();

        bool ShouldTerminate() { return m_isTerminationRequested; };

        bool ImportMesh(std::string filepath, std::string name);

        bool SaveMeshToCooked();

    private:

        // Required Engine Modules
        GL33Renderer* m_editorRenderer;
        AssetManager* m_assetManager;
        Debug* m_debug;
        Scene* m_workingScene;
        RenderWindow* m_renderWindow;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;
        MeshEditorControls* m_editorControls;

        //States
        bool m_isFullScreen;
        bool m_isTerminationRequested;
        int i = 0;

        TriangleMesh* m_importedMesh = nullptr;
    };
};