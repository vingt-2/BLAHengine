#pragma once

#include <Common/StdInclude.h>
#include <Engine/EngineInstance.h>
#include <Engine/Assets/MeshAsset.h>

namespace BLAengine
{
    class CameraController;
    class GameObject;
    class EditorState;
    class TriangleMesh;

    class BLACORE_API EditorSession : public EngineInstance
    {
    public:
        EditorSession(bool external, bool isFullscreen):
            EngineInstance(external, isFullscreen),
            m_frameIndex(0),
            m_lastTimePlayerInteraction(0.f),
            m_autoPlay(true),
            m_lastIkSolveTime(0.f),
            m_testCone(MeshAsset("")),
            m_SkyInvertedSphere(MeshAsset("")),
            m_cameraController(nullptr)
        {}

        void PreEngineUpdate() override;

        void EngineUpdate() override;

        bool InitializeEngine(RenderWindow* renderWindow) override;

        void TerminateEngine() override;

        void EditorDragAndDropedFile(const std::string& filePath) const;

        ~EditorSession() { EngineInstance::~EngineInstance(); };

    private:

        bool LoadNewScene() override;

        bool LoadWorkingScene(std::string filepath) override;

        void MakeSkyObject();

        struct EditorStateRequests
        {
            blaBool m_newSceneRequest = false;
            blaBool m_openSceneRequest = false;
            blaBool m_saveSceneRequest = false;
            blaBool m_saveSceneAsRequest = false;
        } m_editorStateRequests;

        void SetEditorState(EditorState* state);

        void DoTestAnimationDemoStuff();

        /*
         * Editor State
         */
        EditorState* m_editorState;
        CameraController* m_cameraController;
        float m_frameIndex;
        float m_lastTimePlayerInteraction;
        blaF32 m_lastIkSolveTime;
        
        bool m_autoPlay;

        /*
        *  Handle State changing things for the editor
        *
        */
        void HandleLoadScenePrompt();

        void HandleSaveScenePrompt();

        /*
         *  Editor Data
         */
        /*TriangleMesh m_posQuatPositionGizmoMesh;
        TriangleMesh m_posQuatOrientationGizmoMesh;
        TriangleMesh m_posQuatScaleGizmoMesh;*/
        MeshAsset m_testCone;
        MeshAsset m_SkyInvertedSphere;


        /*
         *  Mesh Import Stuff...
         *  
         */
        bool ImportMesh(std::string filepath, std::string name) const;
    };
};