#pragma once

#include <Common/StdInclude.h>
#include <Engine/EngineInstance.h>
#include <Engine/Assets/MeshAsset.h>
#include <Editor/EditorGui/GameObjectInspectorGui.h>
#include <Editor/EditorGui/SceneGraphGui.h>
#include "GizmoManager.h"

namespace BLAengine
{
    class CameraController;
    class GameObject;
    class EditorState;
    class TriangleMesh;

    class BLACORE_API EditorSession : public EngineInstance
    {
    public:
        EditorSession(bool external, bool isFullscreen) :
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

        void PostEngineUpdate() override;

        bool InitializeEngine(RenderWindow* renderWindow) override;

        void TerminateEngine() override;

        void EditorDragAndDropedFile(const blaString& filePath) const;

        ~EditorSession() { EngineInstance::~EngineInstance(); };

        GameObjectReference GetSelectedObject() { return m_selectedObject; }

        void SetSelectedObject(GameObjectReference selectedObject);

    private:

        void DrawGrid(int size, float spacing, const blaVec3& color);

        bool LoadNewScene() override;

        bool LoadWorkingScene(blaString filepath) override;

        void MakeSkyObject();

        void SetEditorState(EditorState* state);

        void EditorUpdate();

        /*
        *  Handle State changing things for the editor
        *
        */
        void HandleEditorStateChangeRequests();

        void HandleLoadScenePrompt();

        void HandleSaveScenePrompt();

        void HandleGuiRequests();

        /*
         *  Mesh Import Stuff...
         *
         */
        bool ImportMesh(blaString filepath, blaString name) const;

    private:

        GameObjectReference m_selectedObject;

        GizmoManager m_gizmoManager;
        GameObjectInspector* m_componentInspector = nullptr;
        SceneGraphGui* m_sceneGraphGui = nullptr;
        /*
         * Editor State
         */
        EditorState* m_editorState;
        CameraController* m_cameraController;
        float m_frameIndex;
        float m_lastTimePlayerInteraction;
        blaF32 m_lastIkSolveTime;

        bool m_autoPlay;

        struct EditorStateRequests
        {
            blaBool m_newSceneRequest = false;
            blaBool m_openSceneRequest = false;
            blaBool m_saveSceneRequest = false;
            blaBool m_saveSceneAsRequest = false;
        } m_editorStateRequests;

        struct EditorGuiRequests
        {
            blaBool m_openConsoleRequest = true;
            blaBool m_openScenGraphGuiRequest = true;
            blaBool m_openComponentInspectorRequest = true;
        } m_editorGuiRequests;

        /*
         *  Editor Data
         */
         /*TriangleMesh m_posQuatPositionGizmoMesh;
         TriangleMesh m_posQuatOrientationGizmoMesh;
         TriangleMesh m_posQuatScaleGizmoMesh;*/
        MeshAsset m_testCone;
        MeshAsset m_SkyInvertedSphere;
    };
};