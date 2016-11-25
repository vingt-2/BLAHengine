#include "EditorSession.h"
using namespace BLAengine;

bool EditorSession::InitializeEngine(RenderWindow* _renderWindow)
{
	this->m_renderWindow = _renderWindow;
	m_assetManager = new AssetManager();

	m_renderingManager = new RenderingManager(RenderingManager::Game);
	m_debugRenderingManager = new DebugRenderingManager();

	m_editorRenderer = new GL33Renderer();

	m_editorRenderer->InitializeRenderer(this->m_renderWindow, m_renderingManager);
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
	m_renderingManager->~RenderingManager();
	m_workingScene->~Scene();
	Scene* scenePtr = m_sceneManager->LoadScene(filepath);
	m_workingScene = scenePtr;

	m_renderingManager = new RenderingManager(RenderingManager::RenderManagerType::Game);
	m_workingScene->Initialize(m_renderingManager);
	m_editorRenderer->SwitchRenderingManager(m_renderingManager);

	return true;
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
