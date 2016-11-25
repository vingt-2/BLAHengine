#include "EditorSession.h"
using namespace BLAengine;

bool EditorSession::InitializeEngine(RenderWindow* _renderWindow)
{
	this->m_renderWindow = _renderWindow;
	m_assetManager = new AssetManager();

	m_editorRenderer = new GL33Renderer();

	m_editorRenderer->InitializeRenderer(this->m_renderWindow);
	m_editorRenderer->m_assetManager = m_assetManager;

	m_workingScene = new Scene();

	m_editorCamera = new Camera();
	GameObject* cameraObject = m_workingScene->CreateObject("Caamera");
	cameraObject->AddComponent(m_editorCamera);
	m_editorRenderer->SetCamera(m_editorCamera);

	m_timer = new Time(10);

	m_workingScene = new Scene();

	m_workingScene->SetTimeObject(m_timer);

	m_sceneManager = new SceneManager(m_assetManager);

	m_renderingManager = new RenderingManager(m_editorRenderer, RenderingManager::Game);
	m_debugRenderingManager = new DebugRenderingManager(m_editorRenderer);

	m_debug = new Debug(m_debugRenderingManager);

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	if (!m_editorRenderer->GetStatus())
	{
		printf("Failed to initiate Context!");
		return false;
	}

	m_assetManager->LoadCookedAssets();

	DirectionalLightRender lr;
	Camera* cameraLight = new Camera();
	GameObject* bla = m_workingScene->CreateObject("ShadowCamera");
	Transform t = bla->GetTransform();
	t.SetRotationUsingEuler(vec3(1, 0, 0));
	bla->SetTransform(t);
	bla->AddComponent(cameraLight);
	lr.m_shadowRender.m_shadowCamera.AttachCamera(cameraLight);
	lr.m_shadowRender.m_shadowCamera.SetOrthographicProj(-200, 200, -200, 200);
	lr.m_shadowRender.m_bufferSize = 8192;
	m_editorRenderer->m_directionalLightsVector.push_back(lr);

	return true;
}

void EditorSession::UpdateEditor()
{
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
	Scene* scenePtr = m_sceneManager->LoadScene(filepath);
	m_workingScene = scenePtr;
	for (auto obj : m_workingScene->GetObjects())
	{
		for (auto meshRenderer : obj->GetComponents<MeshRenderer>())
		{
			std::cout << meshRenderer->m_mesh->GetName() << "\n";
			m_renderingManager->RequestRenderTicket(meshRenderer);
		}
	}
	return true;
}
