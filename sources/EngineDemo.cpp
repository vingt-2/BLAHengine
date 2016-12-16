#include "EngineDemo.h"
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

	m_workingScene = new Scene();

	m_editorScene = new Scene();

	GameObject* cameraObject = m_editorScene->CreateObject("EditorCamera");
	Camera* cameraComp = new Camera();
	cameraObject->AddComponent(cameraComp);
	//m_editorRenderer->SetCamera(cameraComp);

	m_timer = new Time(10);

	m_workingScene = new Scene();

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
	m_debug->DrawRay(debugRay, vec3(0, 1, 0));
	m_debug->DrawGrid(1000, 10, vec3(0.4));
	m_debug->Update();


	//m_timer->Update();
	m_workingScene->Update();
	//m_editorRenderer->Update();

	GameObject* pbrenderObject = m_workingScene->FindNameInScene("PBRCamera");
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

bool BLAengine::Raytracer::LoadWorkingScene(std::string filepath)
{
	m_renderingManager->~RenderingManager();
	m_workingScene->~Scene();
	Scene* scenePtr = m_sceneManager->LoadScene(filepath);
	m_workingScene = scenePtr;

	m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
	m_workingScene->Initialize(m_renderingManager);
	m_editorRenderer->SwitchRenderingManager(m_renderingManager);

	GameObject* light = m_workingScene->CreateObject("DirLight");
	DirectionalLight* dirLight = new DirectionalLight(vec3(0, -1, 0));
	light->AddComponent(dirLight);
	Transform lightT = light->GetTransform();
	lightT.SetRotationUsingEuler(vec3(1.07, 0, 0));
	light->SetTransform(lightT);

	GameObject* pbrender = m_workingScene->CreateObject("PBRCamera");
	Transform t1;
	/*t1.m_position = vec3(5, 1, 3);
	t1.SetRotationUsingEuler(vec3(0.2, -0.75, 0));*/
	t1.m_position = vec3(0, 0.5, 5);
	t1.SetRotationUsingEuler(vec3(0, 0, 0));
	pbrender->SetTransform(t1);
	pbrender->AddComponent(new PBRCamera());

	GameObject* pblight = m_workingScene->CreateObject("Light");
	Transform t;
	t.m_position = vec3(0, 5, -3);
	//t.m_position = vec3(2, -3, -8);
	//t.m_position = vec3(0, 10, -8);
	pblight->SetTransform(t);
	PBRSurface* surface = new PBRSphere(0.5);
	surface->m_material.m_emissivePower = vec3(1000, 1000, 1000);
	pblight->AddComponent(surface);
	pblight->AddComponent(new SphereCollider(0.5));

	for (auto gobject : m_workingScene->GetObjects())
	{
		if (MeshRenderer* mRender = gobject->GetComponent<MeshRenderer>())
		{
			if (!gobject->GetComponent<PBRSurface>())
			{
				if (gobject->GetName() == "sky")
				{
					PBRSurface* pbmRender = new PBRSphere(gobject->GetTransform().m_scale.x);
					pbmRender->m_material.m_brdf = new PBRMaterial::LambertianBRDF();
					//pbmRender->m_material.m_emissivePower = vec3(100000000, 100000000, 100000000);

					gobject->AddComponent(pbmRender);
				}
				else if(gobject->GetName() == "cube")
				{
					PBRSurface* pbmRender = new PBRMesh(mRender->m_mesh);
					//PBRSurface* pbmRender = new PBRSphere(gobject->GetTransform().m_scale.x);
					pbmRender->m_material.m_brdf = new PBRMaterial::MirrorBRDF();
					pbmRender->m_material.m_color = vec3(0.2, 0.2, 1);
					//pbmRender->m_material.m_emissivePower = vec3(5, 5, 40);
					gobject->AddComponent(pbmRender);
				}
				else if (gobject->GetName() == "cube2")
				{
					PBRSurface* pbmRender = new PBRMesh(mRender->m_mesh);
					//PBRSurface* pbmRender = new PBRSphere(gobject->GetTransform().m_scale.x);
					pbmRender->m_material.m_brdf = new PBRMaterial::LambertianBRDF();
					pbmRender->m_material.m_color = vec3(3, 0.1, 0.1);
					//pbmRender->m_material.m_emissivePower = vec3(5, 5, 40);
					gobject->AddComponent(pbmRender);
				}
				else
				{
					PBRSurface* pbmRender = new PBRMesh(mRender->m_mesh);
					//PBRSurface* pbmRender = new PBRSphere(gobject->GetTransform().m_scale.x);
					pbmRender->m_material.m_brdf = new PBRMaterial::LambertianBRDF();
					pbmRender->m_material.m_color = vec3(1, 1, 1);
					gobject->AddComponent(pbmRender);
				}
			}
		}
	}

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
