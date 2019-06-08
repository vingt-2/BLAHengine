#include <Common/System.h>
#include <Common/Maths/Maths.h>
#include <Engine/Renderer/GL33Renderer.h>
#include <Engine/Game/RenderingManager.h>
#include <Engine/Debug/Debug.h>
#include <Engine/Assets/SceneManager.h>
#include <Editor/CameraControl.h>
#include <Engine/Game/GameComponents/AnimationComponent.h>
#include <AssetsImport/ExternalFormats/BVHImport.h>
#include <Engine/Game/GameComponents/IKComponent.h>
#include <Engine/Game/Animation/IK/IKTools.h>
#include <Engine/System/InputManager.h>
#include <Engine/Gui/GuiManager.h>
#include <Engine/Gui/GuiMenu.h>
#include <Engine/Game/GameComponents/ColliderComponent.h>
#include <Engine/Geometry/PrimitiveGeometry.h>
#include <Common/FileSystem/Files.h>
#include <Engine/Game/GameComponents/TestPlayerComponent.h>

#include "EditorSession.h"
#include "AssetsImport/ExternalFormats/OBJImport.h"

using namespace BLAengine;

GameObjectReference g_selectedObject;

void DragAndDropHandler(DragAndDropPayloadDontStore* dragAndDropInput)
{
    for (auto path : *dragAndDropInput)
    {
        std::cout << "Dropped file " << path << "\n";
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
    EngineInstance::PreEngineUpdate();

    if (m_editorState->m_type == EditorState::BLA_EDITOR_EDITING)
    {
        if (m_cameraController)
        {
            m_cameraController->UpdateController();
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
    
    m_debug->DrawGrid(1000, 1, blaVec3(0.85f));

    m_debug->DrawBasis(blaPosQuat::GetIdentity() , 1.f);
}

void EditorSession::EngineUpdate()
{
    if (m_editorState->m_type == EditorState::BLA_EDITOR_EDITING)
    {
        EngineInstance::EngineUpdate();

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
            const std::string currentSceneSavePath = m_sceneManager->GetCurrentSceneFilePath();
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

        DoTestAnimationDemoStuff();
    }
}

bool EditorSession::InitializeEngine(RenderWindow* renderWindow)
{
    if(EngineInstance::InitializeEngine(renderWindow))
    {
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

        /*
         * Create and store gizmo meshes
         */
        LoadNewScene();

        m_testCone = MeshAsset("testCone");
        m_testCone.m_triangleMesh = PrimitiveGeometry::MakeCone(400);

        /*GameObjectReference coneObject = m_workingScene->CreateObject("coneObject");

        BLA_CREATE_COMPONENT(MeshRendererComponent, coneObject);
        coneObject->GetComponent<MeshRendererComponent>()->AssignTriangleMesh(&m_testCone);

        Asset* materialAsset = nullptr;
        if (m_assetManager->GetAsset("BlankDiffuseMat", materialAsset) == AssetManager::AssetType::MaterialAsset)
        {
            coneObject->GetComponent<MeshRendererComponent>()->AssignMaterial((Material*)materialAsset, 0);
        }

        BLA_CREATE_COMPONENT(MeshColliderComponent, coneObject);
        coneObject->GetComponent<MeshColliderComponent>()->SetColliderMesh(&m_testCone.m_triangleMesh);

        BLA_CREATE_COMPONENT(RigidBodyComponent, coneObject);

        BLA_CREATE_COMPONENT(TestPlayerComponent, coneObject);

        coneObject->GetTransform().m_scale = blaVec3(1.f,1.f,1.f);

        coneObject->GetTransform().m_posQuat.GetTranslation().y = 3.f;
        */

        return true;
    }
    return false;
}

bool EditorSession::LoadNewScene()
{
    EngineInstance::LoadNewScene();

    g_selectedObject = GameObjectReference::InvalidReference();

    MakeSkyObject();

    delete m_cameraController;
    m_cameraController = new CameraController(
        m_renderWindow,
        m_workingScene->GetMainCamera(),
        30.f,
        10.0f);

    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(1);
    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(2);


    SetEditorState(new EditorState());

    return true;
}

bool EditorSession::LoadWorkingScene(std::string filepath)
{
    EngineInstance::LoadWorkingScene(filepath);

    g_selectedObject = GameObjectReference::InvalidReference();

    MakeSkyObject();

    GameObjectReference animatedObject = m_workingScene->CreateObject("AnimatedObject");

    BLA_CREATE_COMPONENT(AnimationComponent, animatedObject);

    BLA_CREATE_COMPONENT(IKComponent, animatedObject);

    delete m_cameraController;
    m_cameraController = new CameraController(
        m_renderWindow, 
        m_workingScene->GetMainCamera(),
        30.f,
        10.0f);

    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(1);
    m_renderWindow->SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(2);

    SetEditorState(new EditorState());
    
    return true;
}

void EditorSession::MakeSkyObject()
{
    m_SkyInvertedSphere = MeshAsset("SkySphere");
    m_SkyInvertedSphere.m_triangleMesh = PrimitiveGeometry::MakeSphere(5000);

    GameObjectReference skySphereObject = m_workingScene->CreateObject("SkySphere");

    BLA_CREATE_COMPONENT(MeshRendererComponent, skySphereObject);
    skySphereObject->GetComponent<MeshRendererComponent>()->AssignTriangleMesh(&m_SkyInvertedSphere);

    Asset* materialAsset = nullptr;
    if (m_assetManager->GetAsset("BlankDiffuseMat", materialAsset) == AssetManager::AssetType::MaterialAsset)
    {
        skySphereObject->GetComponent<MeshRendererComponent>()->AssignMaterial((Material*)materialAsset, 0);
    }
}

void EditorSession::TerminateEngine()
{
    EngineInstance::TerminateEngine();
}

void EditorSession::EditorDragAndDropedFile(const std::string& filePath) const
{
    FileEntry file = ParseFilePath(filePath);
    ImportMesh(file.GetFullPath(), file.m_name);
}

void EditorSession::DoTestAnimationDemoStuff()
{
    const InputManager* inputs = InputManager::GetSingletonInstanceRead();

    Ray screenRay = m_renderer->ScreenToRay(m_renderWindow->GetMousePointerScreenSpaceCoordinates());

    ColliderComponent::CollisionContact contactPoint;
    GameObjectReference hoverObject = m_workingScene->PickGameObjectInScene(screenRay, contactPoint);

    if (hoverObject.IsValid() && inputs->GetKeyState(BLA_KEY_LEFT_CONTROL).IsDown())
    {
        m_guiManager->DrawText(hoverObject->GetName(), m_renderWindow->GetMousePositionInWindow());
    }

    GameObjectReference animationObject = m_workingScene->FindObjectByName("AnimatedObject");

    if (animationObject.IsValid())
    {
        if (auto ikCmp = animationObject->GetComponent<IKComponent>())
        {
            if (ikCmp->m_ikChain == nullptr)
            {
                if (auto animCmp = animationObject->GetComponent<AnimationComponent>())
                {
                    if (animCmp->m_animation == nullptr)
                    {
                        animCmp->m_animation = BVHImport::ImportAnimation("./resources/animations/bvh/01_04.bvh")[0];
                        animCmp->m_animation->GetSkeleton()->DiscardJointsByName("Thumb");
                    }

                    vector<blaPosQuat> jointTransformsW;
                    animCmp->m_animation->EvaluateAnimation(0, jointTransformsW);

                    ikCmp->m_ikChain = IKChainJoint::BuildFromSkeleton(animCmp->m_animation->GetSkeleton(), jointTransformsW);
                }
                else
                {
                    ikCmp->m_ikChain = IKChainJoint::CreateTestIKChain2EndsConeTwist(4, 1.f, blaVec3(1.f, 2.f, 0.f));
                }

                int c = 0;
                for (auto endEffector : ikCmp->m_ikChain->GetEndEffectors())
                {
                    GameObjectReference object;
                    if (endEffector->m_joint != nullptr)
                    {
                        object = m_workingScene->CreateObject("EffectorHandles_" + endEffector->m_joint->GetName());
                    }
                    else
                    {
                        object = m_workingScene->CreateObject("EffectorHandles_" + to_string(c));
                    }

                    auto meshCmp = BLA_CREATE_COMPONENT(MeshRendererComponent, object);

                    Asset* sphereMesh = nullptr;
                    this->m_assetManager->GetAsset("sphere", sphereMesh);

                    meshCmp->AssignTriangleMesh((MeshAsset*)sphereMesh);

                    auto meshCollider = BLA_CREATE_COMPONENT(MeshColliderComponent, object);

                    meshCollider->SetColliderMesh(&((MeshAsset*)sphereMesh)->m_triangleMesh);

                    ObjectTransform transform = object->GetTransform();
                    transform.m_scale = blaVec3(0.3f);
                    transform.m_posQuat = endEffector->m_jointTransform;
                    object->SetTransform(transform);

                    c++;
                }
            }
            else
            {
                std::vector<std::pair<blaVec3, blaVec3>> bones;
                std::vector<blaPosQuat> jointTransforms;
                IKChainJoint::GetBoneArray(bones, *ikCmp->m_ikChain);
                IKChainJoint::GetJointTransforms(jointTransforms, *ikCmp->m_ikChain);

                vector<GameObjectReference> effectorHandles = m_workingScene->FindObjectsMatchingName("EffectorHandles_");

                vector<blaVec3> desiredPos;
                for (auto obj : effectorHandles)
                {
                    desiredPos.push_back(obj->GetTransform().GetPosition());
                }

                //if (inputs->GetKeyState(BLA_KEY_SPACE).IsDown())
                {
                    if (m_timer->GetTime() - m_lastIkSolveTime > .005f)
                    {
                        IKChainJoint::SolveIKChain(ikCmp->m_ikChain, desiredPos, 20);

                        m_lastIkSolveTime = m_timer->GetTime();
                    }
                }

                for (size_t i = 0; i < jointTransforms.size(); ++i)
                {
                    m_debug->DrawBasis(jointTransforms[i], 1.f);
                }

                for (size_t i = 0; i < bones.size(); ++i)
                {
                    m_debug->DrawLine(bones[i].first, bones[i].second, blaVec3(1.f, 0.f, 1.f));
                }
            }
        }
        else if (auto animCmp = animationObject->GetComponent<AnimationComponent>())
        {
            if (animCmp->m_animation == nullptr)
            {
                animCmp->m_animation = BVHImport::ImportAnimation("./resources/animations/bvh/01_04.bvh")[0];
            }
            else
            {
                blaF32 animDt = 1.0f / animCmp->m_animation->GetSamplingRate();
                const EngineInstance* engine = GetSingletonInstanceRead();

                blaF32 gameDt = engine->GetTimer()->GetDelta();
                blaF32 animStep = gameDt / animDt;

                if (inputs->GetKeyState(BLA_KEY_RIGHT).IsDown() || inputs->GetKeyState(BLA_KEY_LEFT).IsDown())
                {
                    m_autoPlay = false;
                }

                if (m_autoPlay)
                {
                    m_frameIndex += animStep;
                }
                else
                {
                    if (inputs->GetKeyState(BLA_KEY_RIGHT).IsDown())
                    {
                        m_frameIndex += animStep;
                        m_lastTimePlayerInteraction = engine->GetTimer()->GetTime();
                    }
                    if (inputs->GetKeyState(BLA_KEY_LEFT).IsDown())
                    {
                        m_frameIndex -= animStep;
                        m_frameIndex = m_frameIndex < 0.f ? 0.f : m_frameIndex;
                        m_lastTimePlayerInteraction = engine->GetTimer()->GetTime();
                    }

                    if (engine->GetTimer()->GetTime() - m_lastTimePlayerInteraction > 5.0f)
                    {
                        m_autoPlay = true;
                    }
                }

                m_frameIndex = m_frameIndex >= animCmp->m_animation->GetFrameCount() ? 0.f : m_frameIndex;

                vector<blaPosQuat> jointTransformsW;
                animCmp->m_animation->EvaluateAnimation(static_cast<int>(m_frameIndex), jointTransformsW);

                for (auto jointW : jointTransformsW)
                {
                    GetDebug()->DrawBasis(jointW, 1.f);
                }

                std::vector<std::pair<blaVec3, blaVec3>> bones;

                SkeletonAnimationData::GetBoneArrayFromEvalAnim(bones, animCmp->m_animation->GetSkeleton(), jointTransformsW);

                blaF32 scaleFactor = 0.1f;

                for (size_t i = 0; i < bones.size(); ++i)
                {
                    m_debug->DrawLine(scaleFactor * bones[i].first, scaleFactor * bones[i].second);
                }
            }
        }
    }
    else
    {
        animationObject = m_workingScene->CreateObject("AnimatedObject");

        //BLA_CREATE_COMPONENT(AnimationComponent, animationObject);
        BLA_CREATE_COMPONENT(IKComponent, animationObject);
    }

    auto leftMouseButton = inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_LEFT);
    if (leftMouseButton.IsRisingEdge())
    {
        g_selectedObject = hoverObject;
    }
    if (leftMouseButton.IsFallingEdge())
    {
        g_selectedObject = GameObjectReference();
    }

    if (g_selectedObject.IsValid())
    {
        if (g_selectedObject->GetName().find("EffectorHandles_") != string::npos)
        {
            Ray screenRay = m_renderer->ScreenToRay(m_renderWindow->GetMousePointerScreenSpaceCoordinates());
            GameObjectReference camera = m_workingScene->GetMainCamera()->GetParentObject();

            blaVec3 camUp, camLeft;

            camUp = camera->GetTransform().GetRotation() * blaVec3(0.f, 1.f, 0.f);
            camLeft = camera->GetTransform().GetRotation() * blaVec3(1.f, 0.f, 0.f);

            RaycastHit hit = screenRay.RayToPlaneIntersection(g_selectedObject->GetTransform().GetPosition(), camUp, camLeft);

            ObjectTransform selectedObjectTransform = g_selectedObject->GetTransform();

            if (glm::length(hit.hitPosition - selectedObjectTransform.GetPosition()) > 1.f)
            {

            }
            else
            {
                selectedObjectTransform.SetPosition(hit.hitPosition);

                g_selectedObject->SetTransform(selectedObjectTransform);
            }
        }
    }
}

void EditorSession::HandleSaveScenePrompt()
{
    EditorSaveSceneState* saveState = static_cast<EditorSaveSceneState*>(m_editorState);

    if (saveState->m_currentFileBrowser)
    {
        blaBool shouldCloseBrowser = false;
        std::string savePath;
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

void EditorSession::HandleLoadScenePrompt()
{
    EditorLoadSceneState* state = static_cast<EditorLoadSceneState*>(m_editorState);

    if (state->m_currentFileBrowser)
    {
        std::vector<FileEntry> selectedFiles;
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

bool EditorSession::ImportMesh(std::string filepath, std::string name) const
{
    OBJImport objImporter;

    MeshAsset temporaryMeshAsset(name);

    if (!objImporter.ImportMesh(filepath, temporaryMeshAsset.m_triangleMesh, false, true))
    {
        return false;
    }

    this->m_assetManager->SaveTriangleMesh(&temporaryMeshAsset);
    this->m_assetManager->LoadTriangleMesh(name);

    m_workingScene->DeleteObject("MeshVisualizer");

    GameObjectReference visualizerObject = m_workingScene->CreateObject("MeshVisualizer");
    MeshRendererComponent* meshRenderer = BLA_CREATE_COMPONENT(MeshRendererComponent, visualizerObject);

    Asset* triMeshAsset;

    if (m_assetManager->GetAsset(name, triMeshAsset) == AssetManager::AssetType::TriangleMeshAsset)
    {
        meshRenderer->AssignTriangleMesh((MeshAsset*)triMeshAsset);
    }
    else
    {
        BLA_ASSERT(false);
    }

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

    return true;
}
