#include "MeshImportSession.h"
#include <AssetsImport/ExternalFormats/OBJImport.h>
#include <Engine/System/InputManager.h>

using namespace BLAengine;

bool MeshImportSession::InitializeEngine(RenderWindow* _renderWindow)
{
    this->m_renderWindow = _renderWindow;
    m_assetManager = new AssetManager();

    m_renderingManager = new RenderingManager(RenderingManager::Game);
    m_debugRenderingManager = new DebugRenderingManager();

    m_editorRenderer = new GL33Renderer();

    m_editorRenderer->InitializeRenderer(this->m_renderWindow, m_renderingManager, m_debugRenderingManager);
    m_editorRenderer->m_assetManager = m_assetManager;

    Timer* time = new Timer(10);

    m_workingScene = new Scene();

    GameObject* cameraObject = m_workingScene->CreateObject("EditorCamera");
    CameraComponent* cameraComp = BLA_CREATE_COMPONENT(CameraComponent, cameraObject);
    m_editorRenderer->SetCamera(cameraComp);

    m_debug = new Debug(m_debugRenderingManager);

    bool terminationRequest = false;

    m_workingScene->Initialize(m_renderingManager);

    // OPENGL CONTEXT UP AND RUNNING
    if (!m_editorRenderer->GetStatus())
    {
        printf("Failed to initiate Context!");
        return false;
    }

    m_assetManager->LoadCookedAssets();

    GameObject* light = m_workingScene->CreateObject("DirLight");
    DirectionalLight* dirLight = BLA_CREATE_COMPONENT(DirectionalLight, light);

    Asset* skyMeshAsset = nullptr;
    this->m_assetManager->GetAsset("SkyMesh", skyMeshAsset);

    TriangleMesh* sky = (TriangleMesh*)skyMeshAsset;

    GameObject* ballObj = m_workingScene->CreateObject("sky");
    ObjectTransform t = ballObj->GetTransform();
    t.m_scale = blaVec3(5000, 5000, 5000);
    ballObj->SetTransform(t);
    MeshRendererComponent* meshRender = BLA_CREATE_COMPONENT(MeshRendererComponent, ballObj);
    meshRender->AssignTriangleMesh(sky);

    Asset* materialAsset = nullptr;
    if (m_assetManager->GetAsset("BlankDiffuseMat", materialAsset) == AssetManager::AssetType::MaterialAsset)
    {
        meshRender->AssignMaterial((Material*)materialAsset, 0);
    }
    else
    {
        cout << "Couldn't find Material: " << "BlankDiffuseMat" << "in AssetManager.\n";
    }


    m_editorControls = new MeshEditorControls(cameraObject, light, m_renderWindow);

    return true;
}

void MeshImportSession::UpdateEditor()
{
    m_editorControls->ControlCamera();
    m_workingScene->Update();
    m_editorRenderer->Update();
}

void MeshImportSession::TerminateEditor()
{
    //renderWindow->~GLFWRenderWindow();
}

bool BLAengine::MeshImportSession::ImportMesh(std::string filepath, std::string name)
{
    OBJImport objImporter;

    m_importedMesh = new TriangleMesh(name);

    if (!objImporter.ImportMesh(filepath, *m_importedMesh, false, true))
    {
        return false;
    }
    //this->m_assetManager->SaveTriangleMesh(&newTriangleMesh);
    //this->m_assetManager->LoadTriangleMesh(name);

    m_workingScene->DeleteObject("MeshVisualizer_"+std::to_string(i));

    GameObject* visualizerObject = m_workingScene->CreateObject("MeshVisualizer_" + std::to_string(++i));
    MeshRendererComponent* meshRenderer = BLA_CREATE_COMPONENT(MeshRendererComponent, visualizerObject);
    meshRenderer->AssignTriangleMesh(m_importedMesh);

    Asset* materialAsset = nullptr;
    if (m_assetManager->GetAsset("BlankDiffuseMat", materialAsset) == AssetManager::AssetType::MaterialAsset)
    {
        meshRenderer->AssignMaterial((Material*)materialAsset, 0);
    }
    else
    {
        cout << "Couldn't find Material: " << "BlankDiffuseMat" << "in AssetManager.\n";
    }

    ObjectTransform t = visualizerObject->GetTransform();
    t.SetPosition(blaVec3(0.f, 0.f, -5.f));
    visualizerObject->SetTransform(t);

    m_editorControls->m_cameraObject = visualizerObject;

    return true;
}

bool BLAengine::MeshImportSession::SaveMeshToCooked()
{
    if (!m_importedMesh)
        return false;
    else
        return this->m_assetManager->SaveTriangleMesh(m_importedMesh);
}

void BLAengine::MeshEditorControls::ControlCamera()
{
    auto inputs = InputManager::GetSingletonInstance();

    GameObject* object = nullptr;
    blaVec3* rotation = nullptr;
    bool scale = false;
    bool rotate = false;
    if (inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_LEFT).IsDown() && 
        inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_RIGHT).IsDown())
    {
        scale = true;
        object = m_cameraObject;
        rotation = new blaVec3(0);
    }
    else if (inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_LEFT).IsDown())
    {
        rotate = true;
        object = m_cameraObject;
        rotation = &m_cameraRotation;
    }
    else if (inputs->GetKeyState(BLA_KEY_L).IsDown())
    {
        rotate = true;
        object = m_lightObj;
        rotation = &m_lightRotation;
    }

    if (object == nullptr)
    {
        return;
    }

    ObjectTransform transform = object->GetTransform();

    blaVec2 deltaMouse = inputs->GetMousePointerState().GetDelta();

    blaVec3 deltaRotation = blaVec3(0);

    if(deltaMouse.x > 0)
    {
        deltaRotation.y = 1.f;
    }
    else if (deltaMouse.x < 0)
    {
        deltaRotation.y = -1.f;
    }
    if (deltaMouse.y > 0)
    {
        deltaRotation.x = 1.f;
    }
    else if (deltaMouse.y < 0)
    {
        deltaRotation.x = -1.f;
    }

    *rotation += 0.1f * deltaRotation;
    if (rotate)
        transform.SetEulerAngles(rotation->x, rotation->y, rotation->z);
    
    if (scale)
        transform.m_scale += blaVec3(0.5f*deltaRotation.x);
    object->SetTransform(transform);
    object->Update();
}
