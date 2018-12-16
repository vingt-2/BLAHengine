#include "RaytracerDemo.h"
#include "./Engine/Game/PBRendering/PBRRenderer.h"

using namespace BLAengine;

bool Raytracer::InitializeEngine()
{
    m_assetManager = new AssetManager();

    m_renderingManager = new RenderingManager(RenderingManager::Game);
    m_debugRenderingManager = new DebugRenderingManager();

    m_editorRenderer = new GL33Renderer();

    //m_editorRenderer->InitializeRenderer(this->m_renderWindow, m_renderingManager, m_debugRenderingManager);
    //m_editorRenderer->m_assetManager = m_assetManager;

    GameObject* cameraObject = m_editorScene->CreateObject("EditorCamera");
    CameraComponent* cameraComp = BLA_CREATE_COMPONENT(CameraComponent, cameraObject);
    //m_editorRenderer->SetCamera(cameraComp);

    m_timer = new Time(10);

    m_workingScene = new Scene();

    m_editorScene = new Scene();

    m_workingScene->SetTimeObject(m_timer);

    m_sceneManager = new SceneManager(m_assetManager);

    m_debug = new Debug(m_debugRenderingManager);

    bool terminationRequest = false;

    m_workingScene->Initialize(m_renderingManager);

    m_assetManager->LoadCookedAssets();

    LoadWorkingScene(string("C:/Users/Vingt-2/Desktop/BLASoftware/BLAengine/Scenes/test_scene"));

    return true;
}

void Raytracer::UpdateEditor()
{
    m_debug->DrawRay(debugRay, blaVec3(0.f, 1.f, 0.f));
    m_debug->DrawGrid(1000, 10.f, blaVec3(0.4f));
    m_debug->Update();


    m_timer->Update();
    m_workingScene->Update();
    m_editorRenderer->Update();

    GameObject* pbrenderObject = m_workingScene->FindObjectByName("PBRCamera");
    int i = 0;
    vector<Ray> debugRays;
    if (pbrenderObject)
    {
        if (PBRCamera* renderer = pbrenderObject->GetComponent<PBRCamera>())
        {
            if (m_pbr_render_requested && !renderer->m_shouldRender)
                TerminateEditor();
            else
            {
                renderer->m_shouldRender = true;
                m_pbr_render_requested = true;
            }
        }
    }
}

void Raytracer::TerminateEditor()
{
    m_isTerminationRequested = true;
}

//TODO: Gonna have to finish porting the component system...
bool BLAengine::Raytracer::LoadWorkingScene(std::string filepath)
{
    //m_renderingManager->~RenderingManager();
    //m_workingScene->~Scene();
    //Scene* scenePtr = m_sceneManager->LoadScene(filepath);

    //scenePtr->SetTimeObject(m_timer);

    //m_workingScene = scenePtr;

    //m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
    //m_workingScene->Initialize(m_renderingManager);
    //m_editorRenderer->SwitchRenderingManager(m_renderingManager);

    //GameObject* light = m_workingScene->CreateObject("DirLight");
    //DirectionalLight* dirLight = BLA_CREATE_COMPONENT(DirectionalLight, light);
    //ObjectTransform lightT = light->GetTransform();
    //lightT.SetEulerAngles(1.07f, 0.f, 0.f);
    //light->SetTransform(lightT);

    //GameObject* pbrender = m_workingScene->CreateObject("PBRCamera");
    //BLA_CREATE_COMPONENT(PBRCamera, pbrender);
    //ObjectTransform t1;
    ///*t1.m_position = blaVec3(5, 1, 3);
    //t1.SetEulerAngles(blaVec3(0.2, -0.75, 0));*/
    //t1.SetPosition(blaVec3(0.f, 0.5f, 5.f));
    //t1.SetEulerAngles(0.f, 0.f, 0.f);
    //pbrender->SetTransform(t1);

    //GameObject* pblight = m_workingScene->CreateObject("Light");
    //ObjectTransform t;
    //t.GetPosition() = blaVec3(0, 5, -3);
    ////t.m_position = blaVec3(2, -3, -8);
    ////t.m_position = blaVec3(0, 10, -8);
    //pblight->SetTransform(t);
    //PBRSurfaceComponent* surface = new PBRSphereComponent(0.5);
    //surface->m_material.m_emissivePower = blaVec3(1000, 1000, 1000);
    //BLA_CREATE_COMPONENT()
    //pblight->AddComponent(surface);
    //pblight->AddComponent(new SphereColliderComponent(0.5));


    //for (auto gobject : m_workingScene->GetObjects())
    //{
    //    if (MeshRendererComponent* mRender = gobject->GetComponent<MeshRendererComponent>())
    //    {
    //        if (!gobject->GetComponent<PBRSurfaceComponent>())
    //        {
    //            if (gobject->GetName() == "sky")
    //            {
    //                PBRSurfaceComponent* pbmRender = new PBRSphereComponent(gobject->GetTransform().m_scale.x);
    //                pbmRender->m_material.m_brdf = new PBRMaterial::LambertianBRDF();
    //                //pbmRender->m_material.m_emissivePower = blaVec3(100000000, 100000000, 100000000);

    //                gobject->AddComponent(pbmRender);
    //            }
    //            else if(gobject->GetName() == "sphere")
    //            {
    //                PBRSurfaceComponent* pbmRender = new PBRMeshComponent(mRender->m_mesh);
    //                //PBRSurfaceComponent* pbmRender = new PBRSphereComponent(gobject->GetTransform().m_scale.x);
    //                pbmRender->m_material.m_brdf = new PBRMaterial::MirrorBRDF();
    //                pbmRender->m_material.m_color = blaVec3(0.2, 0.2, 1);
    //                //pbmRender->m_material.m_emissivePower = blaVec3(5, 5, 40);
    //                gobject->AddComponent(pbmRender);
    //            }
    //            else if (gobject->GetName() == "redcube1" || gobject->GetName() == "redcube4")
    //            {
    //                PBRSurfaceComponent* pbmRender = new PBRMeshComponent(mRender->m_mesh);
    //                //PBRSurfaceComponent* pbmRender = new PBRSphereComponent(gobject->GetTransform().m_scale.x);
    //                pbmRender->m_material.m_brdf = new PBRMaterial::LambertianBRDF();
    //                pbmRender->m_material.m_color = blaVec3(1, 0.1, 0.1);
    //                //pbmRender->m_material.m_emissivePower = blaVec3(5, 5, 40);
    //                gobject->AddComponent(pbmRender);
    //            }
    //            else if (gobject->GetName() == "redcube2" || gobject->GetName() == "redcube3")
    //            {
    //                PBRSurfaceComponent* pbmRender = new PBRMeshComponent(mRender->m_mesh);
    //                //PBRSurfaceComponent* pbmRender = new PBRSphereComponent(gobject->GetTransform().m_scale.x);
    //                pbmRender->m_material.m_brdf = new PBRMaterial::LambertianBRDF();
    //                pbmRender->m_material.m_color = blaVec3(1, 0.1, 0.1);
    //                //pbmRender->m_material.m_emissivePower = blaVec3(5, 5, 40);
    //                gobject->AddComponent(pbmRender);
    //            }
    //            else
    //            {
    //                PBRSurfaceComponent* pbmRender = new PBRMeshComponent(mRender->m_mesh);
    //                //PBRSurfaceComponent* pbmRender = new PBRSphereComponent(gobject->GetTransform().m_scale.x);
    //                pbmRender->m_material.m_brdf = new PBRMaterial::LambertianBRDF();
    //                pbmRender->m_material.m_color = blaVec3(1, 1, 1);
    //                gobject->AddComponent(pbmRender);
    //            }
    //        }
    //    }
    //}

    return true;
}

bool BLAengine::Raytracer::SaveWorkingScene(std::string filepath)
{
    return m_sceneManager->SaveScene(filepath, m_workingScene);
}

std::vector<string> BLAengine::Raytracer::GetSceneObjects()
{
    std::vector<string> objs;
    for (auto go : m_workingScene->GetObjects())
    {
        objs.push_back(go->GetName());
    }
    return objs;
}
