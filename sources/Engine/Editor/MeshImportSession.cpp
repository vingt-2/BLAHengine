#include "MeshImportSession.h"
#include "../../AssetsImport/OBJImport.h"

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

	m_workingScene = new Scene();

	GameObject* cameraObject = m_workingScene->CreateObject("EditorCamera");
	Camera* cameraComp = new Camera();
	cameraObject->AddComponent(cameraComp);
	m_editorRenderer->SetCamera(cameraComp);

	m_workingScene = new Scene();

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
	DirectionalLight* dirLight = new DirectionalLight(vec3(1, 0, 0));
	light->AddComponent(dirLight);

	Asset* skyMeshAsset = nullptr;
	this->m_assetManager->GetAsset("SkyMesh", skyMeshAsset);

	TriangleMesh* sky = (TriangleMesh*)skyMeshAsset;

	GameObject* ball_obj = m_workingScene->CreateObject("sky");
	Transform t = ball_obj->GetTransform();
	t.m_scale = vec3(5000, 5000, 5000);
	ball_obj->SetTransform(t);
	MeshRenderer* meshRender = new MeshRenderer();
	ball_obj->AddComponent(meshRender);
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
	MeshRenderer* meshRenderer = new MeshRenderer();
	meshRenderer->AssignTriangleMesh(m_importedMesh);
	visualizerObject->AddComponent(meshRenderer);

	Asset* materialAsset = nullptr;
	if (m_assetManager->GetAsset("BlankDiffuseMat", materialAsset) == AssetManager::AssetType::MaterialAsset)
	{
		meshRenderer->AssignMaterial((Material*)materialAsset, 0);
	}
	else
	{
		cout << "Couldn't find Material: " << "BlankDiffuseMat" << "in AssetManager.\n";
	}

	Transform t = visualizerObject->GetTransform();
	t.m_position = vec3(0, 0, -5);
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
	GameObject* object = nullptr;
	vec3* rotation = nullptr;
	bool scale = false;
	bool rotate = false;
	if (m_renderWindow->GetMousePressed(1) && (m_renderWindow->GetMousePressed(0)))
	{
		scale = true;
		object = m_cameraObject;
		rotation = new vec3(0);
	}
	else if (m_renderWindow->GetMousePressed(1))
	{
		rotate = true;
		object = m_cameraObject;
		rotation = &m_cameraRotation;
	}
	else if (m_renderWindow->GetKeyPressed('L'))
	{
		rotate = true;
		object = m_lightObj;
		rotation = &m_lightRotation;
	}

	if (object == nullptr)
	{
		return;
	}

	Transform transform = object->GetTransform();

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


	*rotation += 0.01f * deltaRotation;
	if (rotate)
		transform.SetRotationUsingEuler(*rotation);
	
	if (scale)
		transform.m_scale += vec3(0.05*deltaRotation.x);
	object->SetTransform(transform);
	object->Update();
}
