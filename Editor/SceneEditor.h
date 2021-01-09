// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "StdInclude.h"
#include "EngineInstance.h"
#include "Assets/MeshAsset.h"
#include "EditorGui/GameObjectInspectorGui.h"
#include "EditorGui/SceneGraphGui.h"
#include "EditorGui/RenderPassEditorGui.h"
#include "EditorCommands.h"
#include "GizmoManager.h"



namespace BLA
{
    namespace Core
    {
        class GameObject;
    }

    class CameraController;
    class TriangleMesh;
    struct EditorState;
    class DevGuiTextEditor;

    class SceneEditor : public EngineInstance
    {
    public:
	    SceneEditor(bool external, bool isFullscreen);

		bool InitializeEngine(RenderWindow* renderWindow) override;

        void PreEngineUpdate() override;
        void EngineUpdate() override;
        void PostEngineUpdate() override;
        void TerminateEngine() override;

		void InitializeComponentLibrariesManager() override;

        void EditorDragAndDropedFile(const blaString& filePath);

	    ~SceneEditor();

        Core::GameObject GetSelectedObject() { return m_selectedObject; }

        void SetSelectedObject(Core::GameObject selectedObject);

        CameraController* GetCameraController() const { return m_cameraController; }

        EditorCommandManager* GetCommandManager() const { return m_commandManager; }

        /*
         * Scene Updated temp logic
         *
         */

        void SetObjectParent(Core::GameObject parent, Core::GameObject child);

    	//TODO: This whole thing is messed up. Less singletons, more component storage
        bool TemporaryComponentEdit(const GameComponentEditCommand* editCommand);

    private:

        void DrawGrid(int size, float spacing, const blaVec3& color) const;

        bool LoadNewScene() override;

        bool LoadWorkingScene(blaString filepath) override;

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
         */
        bool ImportMesh(blaString filepath, blaString name) const;

    private:
        bool m_updatedScene = false;
        bool m_bDrawGrid = false;
        bool m_bTextEdit = false;
    	
        Core::GameObject m_selectedObject;

        GizmoManager* m_gizmoManager;
        GameObjectInspector* m_componentInspector = nullptr;
        RenderPassEditorGui* m_renderPassEditor = nullptr;
        SceneGraphGui* m_sceneGraphGui = nullptr;
        /*
         * Editor State
         */
        EditorState* m_editorState;
		CameraController* m_cameraController;
        EditorCommandManager* m_commandManager;

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
            blaBool m_openRenderPassEditorRequest = false;
        } m_editorGuiRequests;
    };
};