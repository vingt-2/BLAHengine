#include "EditorSession.h"
#include "../Game/PBRendering/PBRRenderer.h"

using namespace BLAengine;

bool EditorSession::InitializeEngine(RenderWindow* _renderWindow)
{
	this->m_renderWindow = _renderWindow;
	m_assetManager = new AssetManager();

	m_renderingManager = new RenderingManager(RenderingManager::Game);
	m_debugRenderingManager = new DebugRenderingManager();

	m_editorRenderer = new GL33Renderer();

	m_editorRenderer->InitializeRenderer(this->m_renderWindow, m_renderingManager, m_debugRenderingManager);
	m_editorRenderer->m_assetManager = m_assetManager;

	m_workingScene = new Scene();

	m_editorScene = new Scene();

	GameObject* cameraObject = m_editorScene->CreateObject("EditorCamera");
	Camera* cameraComp = new Camera();
	cameraObject->AddComponent(cameraComp);
	m_editorRenderer->SetCamera(cameraComp);

	m_timer = new Time(10);

	m_workingScene = new Scene();

	m_workingScene->SetTimeObject(m_timer);

	m_sceneManager = new SceneManager(m_assetManager);

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

	m_editorControls = new EditorControls(cameraObject, m_renderWindow);

	return true;
}

void EditorSession::UpdateEditor()
{
	if (m_renderWindow->GetMousePressed(0))
	{
		Ray screenRay = m_editorRenderer->ScreenToRay();
		vec3 t;
		if (GameObject* object = m_workingScene->PickGameObjectInScene(screenRay).first)
		{
			std::cout << "Picked " << object->GetName() << "\n";
			Collider::RayCollision cp = object->GetComponent<Collider>()->CollideWithRay(screenRay);
			debugRay = Ray(cp.m_colPositionW, cp.m_colNormalW, 10000);
		}
	}

	if (GameObject* dirlightObj = m_workingScene->FindNameInScene("DirLight"))
	{
		if (DirectionalLight* dirLight = dirlightObj->GetComponent<DirectionalLight>())
		{
			Transform lightT = dirLight->GetObjectTransform();
			lightT.m_position = vec3(0, 50, 0);
			vec3 rotationInEuler = lightT.GetEulerRotation();
			cout << rotationInEuler.y;
			lightT.SetRotationUsingEuler(vec3(rotationInEuler.x + 0.01, 0 ,0));
			dirlightObj->SetTransform(lightT);
		}
	}

	m_debug->DrawRay(debugRay, vec3(0, 1, 0));
	m_debug->DrawGrid(1000, 10, vec3(0.4));
	m_debug->Update();

	m_editorControls->ControlCamera();
	m_timer->Update();
	m_workingScene->Update();
	m_editorRenderer->Update();
}

void EditorSession::TerminateEditor()
{
	//renderWindow->~GLFWRenderWindow();
}

bool BLAengine::EditorSession::LoadWorkingScene(std::string filepath)
{
	m_renderingManager->~RenderingManager();
	m_workingScene->~Scene();
	Scene* scenePtr = m_sceneManager->LoadScene(filepath);
	m_workingScene = scenePtr;

	m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
	m_workingScene->Initialize(m_renderingManager);
	m_editorRenderer->SwitchRenderingManager(m_renderingManager);

	GameObject* light = m_workingScene->CreateObject("DirLight");
	DirectionalLight* dirLight = new DirectionalLight(vec3(0, 10, 0));
	light->AddComponent(dirLight);
	Transform lightT = light->GetTransform();
	lightT.m_position = vec3(0, 20, 0);
	lightT.SetRotationUsingEuler(vec3(0, 1.2, 0));
	light->SetTransform(lightT);

	return true;
}

bool BLAengine::EditorSession::SaveWorkingScene(std::string filepath)
{
	return m_sceneManager->SaveScene(filepath, m_workingScene);
}

std::vector<string> BLAengine::EditorSession::GetSceneObjects()
{
	std::vector<string> objs;
	for (auto go : m_workingScene->GetObjects())
	{
		objs.push_back(go->GetName());
	}
	return objs;
}

void BLAengine::EditorControls::ControlCamera()
{
	Transform transform = m_cameraObject->GetTransform();

	vec3 tangentForce = vec3(0);
	vec3 movementForce = vec3(0);
	float coeff = 0.01f;


	if (m_renderWindow->GetKeyPressed('W'))
		tangentForce.z = 2.f;
	if (m_renderWindow->GetKeyPressed('S'))
		tangentForce.z = -2.f;

	if (m_renderWindow->GetKeyPressed('A'))
		tangentForce.x = 2.f;
	if (m_renderWindow->GetKeyPressed('D'))
		tangentForce.x = -2.f;

	if (m_renderWindow->GetKeyPressed('Q'))
		tangentForce.y = 2.f;
	if (m_renderWindow->GetKeyPressed('E'))
		tangentForce.y = -2.f;

	vec3 cameraForce = transform.LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;
	transform.m_position += cameraForce;


	if (m_renderWindow->GetMousePressed(1))
	{
		double x, y;
		m_renderWindow->GetMouse(x, y);
		vec2 curMouse = vec2(x, y);

		vec3 deltaRotation = vec3(0);

		if (x - m_prevMouse.x > 0)
		{
			deltaRotation.y = 1.f;
		}
		else if (x - m_prevMouse.x < 0)
		{
			deltaRotation.y = -1.f;
		}
		if (y - m_prevMouse.y > 0)
		{
			deltaRotation.x = 1.f;
		}
		else if (y - m_prevMouse.y < 0)
		{
			deltaRotation.x = -1.f;
		}

		m_prevMouse = curMouse;

		m_cameraRotation += 0.05f * deltaRotation;
		
		transform.SetRotationUsingEuler(m_cameraRotation);
	}

	m_cameraObject->SetTransform(transform);

	m_cameraObject->Update();
}
