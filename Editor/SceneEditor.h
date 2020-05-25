#pragma once

#include "StdInclude.h"
#include "EngineInstance.h"
#include "Assets/MeshAsset.h"
#include "EditorGui/GameObjectInspectorGui.h"
#include "EditorGui/SceneGraphGui.h"
#include "EditorCommands.h"
#include "GizmoManager.h"

namespace BLA
{
    class CameraController;
    class GameObject;
    class TriangleMesh;
    struct EditorState;

	
    /*
     * Vulkan Test
     */
    class VulkanRenderer;
    class GLFWRenderWindow;
	
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

        GameObject GetSelectedObject() { return m_selectedObject; }

        void SetSelectedObject(GameObject selectedObject);

        CameraController* GetCameraController() const { return m_cameraController; }

        EditorCommandManager* GetCommandManager() const { return m_commandManager; }

        /*
         * Scene Updated temp logic
         *
         */

        void SetObjectParent(GameObject parent, GameObject child);

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
        bool m_bTestVulkan = false;

    	/*
    	 * Vulkan Test
    	 */
        GLFWRenderWindow* m_vulkanWindow = nullptr;
        VulkanRenderer* m_vulkanRenderer = nullptr;
    	
        GameObject m_selectedObject;

        GizmoManager* m_gizmoManager;
        GameObjectInspector* m_componentInspector = nullptr;
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
        } m_editorGuiRequests;
    };
};