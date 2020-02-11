#include "System.h"
#include "Maths/Maths.h"
#include "Renderer/GL33Renderer.h"
#include "Core/Timer.h"
#include "Core/DebugDraw.h"
#include "Assets/SceneManager.h"
#include "CameraControl.h"
#include "System/Console.h"
#include "System/InputManager.h"
#include "Gui/GuiManager.h"
#include "Gui/GuiMenu.h"
#include "Physics/ColliderComponent.h"
#include "Geometry/PrimitiveGeometry.h"
#include "System/FileSystem/Files.h"
#include "Renderer/MeshRendererComponent.h"
#include "EditorComponentLibrariesManager.h"
#include "AssetsImport/OBJImport.h"
#include "Core/TransformComponent.h"
#include "EditorSession.h"

using namespace BLAengine;

BLA_CONSOLE_VAR(bool, shouldReloadLibraries, false)

BLA_CONSOLE_COMMAND(void, MakeSkyObject)
{
    Asset* m;
    if (AssetManager::GetSingletonInstance()->GetAsset("SkySphere", m) != AssetManager::TriangleMeshAsset)
    {
        MeshAsset skyInvertedSphere = MeshAsset("SkySphere");
        skyInvertedSphere.m_triangleMesh = PrimitiveGeometry::MakeSphere(5000);
        AssetManager::GetSingletonInstance()->SaveTriangleMesh(&skyInvertedSphere);
    }
    GameObject skySphereObject = Scene::GetSingletonInstance()->CreateObject(BlaStringId("Sky Sphere"));

    skySphereObject.CreateComponent<MeshRendererComponent>()->MeshAssetName = "SkySphere";
    skySphereObject.GetComponent<MeshRendererComponent>()->MaterialName = "BlankDiffuseMat";
}

void DragAndDropHandler(DragAndDropPayloadDontStore* dragAndDropInput)
{
    for (auto path : *dragAndDropInput)
    {
        Console::LogMessage("Dropped file " + path);
        static_cast<EditorSession*>(EngineInstance::GetSingletonInstance())->EditorDragAndDropedFile(path); // ``safe'' cast...
    }
}

namespace BLAengine
{
    struct EditorState
    {
        enum StateType
        {
            BLA_EDITOR_EDITING,
            BLA_EDITOR_LOAD_SCENE_PROMPT,
            BLA_EDITOR_SAVE_SCENE_PROMPT
        } m_type = BLA_EDITOR_EDITING;
    };

    struct EditorLoadSceneState : EditorState
    {
        EditorLoadSceneState()
        {
            m_type = BLA_EDITOR_LOAD_SCENE_PROMPT;
            m_currentFileBrowser = nullptr;
        }

        const OpenFilePrompt* m_currentFileBrowser;
    };

    struct EditorSaveSceneState : EditorState
    {
        EditorSaveSceneState()
        {
            m_type = BLA_EDITOR_SAVE_SCENE_PROMPT;
            m_currentFileBrowser = nullptr;
        }

        const SaveFilePrompt* m_currentFileBrowser;
    };
}


void EditorSession::SetEditorState(EditorState* state)
{
    delete m_editorState;
    m_editorState = state;
}

void EditorSession::PreEngineUpdate()
{
    if(g_shouldReloadLibraries) 
    {
        static_cast<EditorComponentLibrariesManager*>(m_componentLibrariesManager)->ReloadLibraries();
        g_shouldReloadLibraries = false;
    }

    EngineInstance::PreEngineUpdate();

    if (m_editorState->m_type == EditorState::BLA_EDITOR_EDITING)
    {
        if (m_cameraController)
        {
            m_cameraController->UpdateController(m_timer->GetDelta());
        }
    }
    else if (m_editorState->m_type == EditorState::BLA_EDITOR_LOAD_SCENE_PROMPT)
    {
        HandleLoadScenePrompt();
    }
    else if (m_editorState->m_type == EditorState::BLA_EDITOR_SAVE_SCENE_PROMPT)
    {
        HandleSaveScenePrompt();
    }

    if (InputManager::GetSingletonInstance()->GetKeyState(BLA_KEY_GRAVE_ACCENT).IsRisingEdge())
    {
        m_editorGuiRequests.m_openConsoleRequest = ~m_editorGuiRequests.m_openConsoleRequest;
    }
}

void EditorSession::EngineUpdate()
{
    if (m_editorState->m_type == EditorState::BLA_EDITOR_EDITING)
    {
        EditorUpdate();

        if (m_sceneGraphGui && (m_scene->GetSceneFlags() & Scene::ESceneFlags::DIRTY_SCENE_STRUCTURE) != 0)
        {
            m_sceneGraphGui->UpdateSceneGraph();
        }

        EngineInstance::EngineUpdate();

        HandleEditorStateChangeRequests();

        HandleGuiRequests();

        if (m_inputManager->GetKeyState(BLA_KEY_ESCAPE).IsRisingEdge())
        {
            m_selectedObject = GameObject::InvalidReference();
        }
    }
}

//TODO: Don't call the same things as Engine ...
void EditorSession::PostEngineUpdate()
{
    m_gizmoManager.Update();

    m_renderer->Update();

    m_guiManager->Update();

    // Inputs should be the second to last thing to update !
    m_inputManager->Update();

    if (!m_renderWindow->HasCapturedMouse())
    {
        m_inputManager->m_lockMouse = m_guiManager->IsMouseOverGui();
        m_inputManager->m_lockKeyboard = m_guiManager->IsMouseOverGui();

        if (const BlaGuiRenderWindow * renderGuiWindow = dynamic_cast<const BlaGuiRenderWindow*>(m_guiManager->GetWindow("Editor Window")))
        {
            if (renderGuiWindow->HasFocus())
            {
                blaVec2 mouseCoord = renderGuiWindow->GetMousePointerScreenSpaceCoordinates();
                if (mouseCoord.x >= 0.f && mouseCoord.x <= 1.f && mouseCoord.y >= 0.f && mouseCoord.y <= 1.f)
                {
                    InputManager::GetSingletonInstance()->m_lockKeyboard = false;
                    InputManager::GetSingletonInstance()->m_lockMouse = false;
                }
            }
        }
    }

    // Final update of the frame
    m_renderWindow->UpdateWindowAndBuffers();
}

bool EditorSession::InitializeEngine(RenderWindow* renderWindow)
{
    if (EngineInstance::InitializeEngine(renderWindow))
    {
        m_renderer->SetRenderToFrameBufferOnly(true);

        m_guiManager->OpenWindow("Editor Window", new BlaGuiRenderWindow(m_renderer, "Editor Window", blaVec2(0.f, 0.f)));

        m_renderWindow->SetDragAndDropCallback((DragAndDropCallback)DragAndDropHandler);

        m_editorState = new EditorState();
        m_editorState->m_type = EditorState::BLA_EDITOR_EDITING;

        /*
         * Create The menu
         */
        BlaGuiMenuTab fileMenu("File");

        fileMenu.AddMenu(BlaGuiMenuItem("New Scene", &m_editorStateRequests.m_newSceneRequest));
        fileMenu.AddMenu(BlaGuiMenuItem("Open Scene", &m_editorStateRequests.m_openSceneRequest, true));
        fileMenu.AddMenu(BlaGuiMenuItem("Save", &m_editorStateRequests.m_saveSceneRequest));
        fileMenu.AddMenu(BlaGuiMenuItem("Save As", &m_editorStateRequests.m_saveSceneAsRequest, true));
        fileMenu.AddMenu(BlaGuiMenuItem("Exit", &(m_isTerminationRequested)));

        m_guiManager->m_menuBar.m_menuTabs.push_back(fileMenu);

        BlaGuiMenuTab settingsMenu("Settings");
        settingsMenu.AddMenu(BlaGuiMenuItem("G-Buffer", &m_renderer->m_debugDrawGBuffer));

        m_guiManager->m_menuBar.m_menuTabs.push_back(settingsMenu);

        BlaGuiMenuTab windowsMenu("Windows");
        windowsMenu.AddMenu(BlaGuiMenuItem("Console", &m_editorGuiRequests.m_openConsoleRequest));
        windowsMenu.AddMenu(BlaGuiMenuItem("Scene Graph", &m_editorGuiRequests.m_openScenGraphGuiRequest));
        windowsMenu.AddMenu(BlaGuiMenuItem("Component Inspector", &m_editorGuiRequests.m_openComponentInspectorRequest));

        m_guiManager->m_menuBar.m_menuTabs.push_back(windowsMenu);

        LoadNewScene();

        return true;
    }
    return false;
}

bool EditorSession::LoadNewScene()
{
    EngineInstance::LoadNewScene();

    m_selectedObject = GameObject::InvalidReference();

    // MakeSkyObject();

    delete m_cameraController;
    m_cameraController = new CameraController(
        m_renderWindow,
        30.f,
        10.0f);

    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(1);
    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(2);

    SetEditorState(new EditorState());

    return true;
}

bool EditorSession::LoadWorkingScene(blaString filepath)
{
    EngineInstance::LoadWorkingScene(filepath);

    m_selectedObject = GameObject::InvalidReference();

    delete m_cameraController;
    m_cameraController = new CameraController(
        m_renderWindow,
        30.f,
        10.0f);

    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(1);
    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(2);

    SetEditorState(new EditorState());

    return true;
}

void EditorSession::TerminateEngine()
{
    EngineInstance::TerminateEngine();
}

void EditorSession::InitializeComponentLibrariesManager()
{
	m_componentLibrariesManager = new EditorComponentLibrariesManager();
}

void EditorSession::EditorDragAndDropedFile(const blaString& filePath) const
{
    FileEntry file = ParseFilePath(filePath);
    ImportMesh(file.GetFullPath(), file.m_name);
}

void EditorSession::EditorUpdate()
{
    const InputManager* inputs = InputManager::GetSingletonInstanceRead();

    Ray screenRay;
    if (const BlaGuiRenderWindow* guiRenderWindow = dynamic_cast<const BlaGuiRenderWindow*>(m_guiManager->GetWindow("Editor Window")))
    {
        screenRay = m_renderer->ScreenToRay(guiRenderWindow->GetMousePointerScreenSpaceCoordinates());

        ColliderComponent::CollisionContact contactPoint;
        GameObject hoverObject;// = m_scene->PickGameObjectInScene(screenRay, contactPoint);

        auto leftMouseButton = inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_LEFT);
        if (leftMouseButton.IsRisingEdge())
        {
            SetSelectedObject(hoverObject);
        }
    }

    if (m_selectedObject.IsValid())
    {
        if (MeshRendererComponent* meshRenderer = m_selectedObject.GetComponent<MeshRendererComponent>())
        {
            //DebugDraw::DrawArbitraryGeometry(m_selectedObject->GetTransform().GetPosQuat(), m_selectedObject->GetTransform().GetScale(), meshRenderer->m_mesh->m_triangleMesh, blaVec4(BLAColors::ORANGE, 0.3f));
        }
    }

    DebugDraw::DrawBasis(blaPosQuat::GetIdentity(), 1.f);
}

void EditorSession::HandleSaveScenePrompt()
{
    EditorSaveSceneState* saveState = static_cast<EditorSaveSceneState*>(m_editorState);

    if (saveState->m_currentFileBrowser)
    {
        blaBool shouldCloseBrowser = false;
        blaString savePath;
        if (saveState->m_currentFileBrowser->GetConfirmedSavePath(savePath))
        {
            SaveWorkingScene(savePath);
            shouldCloseBrowser = true;
        }
        else if (saveState->m_currentFileBrowser->IsBrowsingCancelled())
        {
            shouldCloseBrowser = true;
        }

        if (shouldCloseBrowser)
        {
            m_guiManager->CloseFileBrowser("Save Scene");
            SetEditorState(new EditorState());
        }
    }
    else
    {
        saveState->m_currentFileBrowser = m_guiManager->CreateSaveFilePrompt("Save Scene");
    }
}

void EditorSession::HandleGuiRequests()
{
    if (m_editorGuiRequests.m_openConsoleRequest)
    {
        m_guiManager->OpenConsole("Console");
        m_editorGuiRequests.m_openConsoleRequest = false;
    }
    if (m_editorGuiRequests.m_openScenGraphGuiRequest)
    {
        if (!m_sceneGraphGui)
        {
            m_sceneGraphGui = new SceneGraphGui();
            m_sceneGraphGui->UpdateSceneGraph();
        }
        m_editorGuiRequests.m_openScenGraphGuiRequest = false;
    }
    if (m_editorGuiRequests.m_openComponentInspectorRequest)
    {
        if (!m_componentInspector) m_componentInspector = new GameObjectInspector();

        m_editorGuiRequests.m_openComponentInspectorRequest = false;
    }
}

void EditorSession::HandleEditorStateChangeRequests()
{
    if (m_editorStateRequests.m_openSceneRequest)
    {
        m_editorStateRequests.m_openSceneRequest = false;
        SetEditorState(new EditorLoadSceneState());
    }

    if (m_editorStateRequests.m_newSceneRequest)
    {
        m_editorStateRequests.m_newSceneRequest = false;
        LoadNewScene();
    }

    if (m_editorStateRequests.m_saveSceneAsRequest)
    {
        m_editorStateRequests.m_saveSceneAsRequest = false;
        SetEditorState(new EditorSaveSceneState());
    }

    if (m_editorStateRequests.m_saveSceneRequest)
    {
        const blaString currentSceneSavePath = m_sceneManager->GetCurrentSceneFilePath();
        m_editorStateRequests.m_saveSceneRequest = false;
        if (currentSceneSavePath.empty())
        {
            SetEditorState(new EditorSaveSceneState());
        }
        else
        {
            SaveWorkingScene(currentSceneSavePath);
        }
    }
}

void EditorSession::HandleLoadScenePrompt()
{
    EditorLoadSceneState* state = static_cast<EditorLoadSceneState*>(m_editorState);

    if (state->m_currentFileBrowser)
    {
        blaVector<FileEntry> selectedFiles;
        blaBool shouldCloseBrowser = false;
        if (state->m_currentFileBrowser->GetConfirmedSelection(selectedFiles))
        {
            if (!selectedFiles.empty())
            {
                LoadWorkingScene(selectedFiles[0].m_path);
                shouldCloseBrowser = true;
            }
        }
        else if (state->m_currentFileBrowser->IsBrowsingCancelled())
        {
            shouldCloseBrowser = true;
        }

        if (shouldCloseBrowser)
        {
            m_guiManager->CloseFileBrowser("Load Scene File");
            SetEditorState(new EditorState());
        }
    }
    else
    {
        state->m_currentFileBrowser = m_guiManager->CreateOpenFilePrompt("Load Scene File", true);
    }
}

bool EditorSession::ImportMesh(blaString filepath, blaString name) const
{
    {
        OBJImport objImporter;
        MeshAsset temporaryMeshAsset(name);

        if (!objImporter.ImportMesh(filepath, temporaryMeshAsset.m_triangleMesh, false, true))
        {
            return false;
        }

        this->m_assetManager->SaveTriangleMesh(&temporaryMeshAsset);
    }

    this->m_assetManager->LoadTriangleMesh(name);

    GameObject visualizerObject = m_scene->CreateObject(GenerateBlaStringId(name));
    MeshRendererComponent* meshRenderer = visualizerObject.CreateComponent<MeshRendererComponent>();
    //MeshColliderComponent* colliderComp = visualizerObject.CreateComponent<MeshColliderComponent>();

    meshRenderer->MeshAssetName = name;
    meshRenderer->MaterialName = "BlankDiffuseMat";

    return true;
}

void EditorSession::SetSelectedObject(GameObject selectedObject)
{
    if (m_selectedObject != selectedObject)
    {
        if (selectedObject.IsValid())
            m_componentInspector->InspectGameObject(selectedObject);
    }

    if (selectedObject.IsValid())
        m_selectedObject = selectedObject;
}

void EditorSession::DrawGrid(int size, float spacing, const blaVec3& color)
{
    for (int i = -size / 2; i <= size / 2; i++)
    {
        float iSpacing = i * spacing;
        float sizeSpacing = size * spacing;
        DebugDraw::DrawLine(blaVec3(sizeSpacing / 2, 0, iSpacing), blaVec3(-sizeSpacing / 2, 0, iSpacing), color);
        DebugDraw::DrawLine(blaVec3(iSpacing, 0, sizeSpacing / 2), blaVec3(iSpacing, 0, -sizeSpacing / 2), color);
    }
}

BLA_CONSOLE_COMMAND(void, SelectObject, blaString name)
{
    GameObject obj = EngineInstance::GetSingletonInstance()->GetWorkingScene()->FindObjectByName(name);

    if (obj.IsValid())
    {
        if (EditorSession* editorSession = dynamic_cast<EditorSession*>(EngineInstance::GetSingletonInstance()))
        {
            editorSession->SetSelectedObject(obj);
        }
    }
}

BLA_CONSOLE_COMMAND(int, SelectScale, blaString name, blaF32 scalex, blaF32 scaley, blaF32 scalez)
{
    GameObject obj = EngineInstance::GetSingletonInstance()->GetWorkingScene()->FindObjectByName(name);

    if (obj.IsValid())
    {
        blaScaledTransform s = obj.GetComponent<TransformComponent>()->GetTransform();
        s.m_scale = blaVec3(scalex, scaley, scalez);
        obj.GetComponent<TransformComponent>()->SetTransform(s);
    }
    return 0;
}

BLA_CONSOLE_COMMAND(void, CreatePointLight, blaString name)
{
    GameObject obj = EngineInstance::GetSingletonInstance()->GetWorkingScene()->CreateObject(GenerateBlaStringId(name));
    obj.CreateComponent(BlaStringId("PointLightComponent"));
}


BLA_CONSOLE_COMMAND(void, AddComponent, blaString objectName, blaString componentName)
{
	GameObject obj(GenerateBlaStringId(objectName));
	if(obj.IsValid()) 
    {
        obj.CreateComponent(GenerateBlaStringId(componentName));
	}
}

BLA_CONSOLE_COMMAND(void, CreateObject, blaString objectName)
{
    Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();
    scene->CreateObject(GenerateBlaStringId(objectName));
}