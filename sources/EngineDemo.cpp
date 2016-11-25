#include "EngineDemo.h"
using namespace BLAengine;

vec2 previousMouseInput;
vec3 cameraRotation;

void SetObject(GameObject* object)
{
	Transform transform;

	transform.m_position = (vec3(-10, 0, 0));

	transform.SetRotationUsingEuler(vec3(1, 1, 1));

	object->SetTransform(transform);


	if (RigidBody* rgbdy = object->GetComponent<RigidBody>())
	{
		rgbdy->m_angularVelocity = vec3(0);
		rgbdy->m_velocity= vec3(0);
	}
}

void SimpleControls(GameObject* object, RenderWindow* renderWindow)
{

	Transform transform = object->GetTransform();

	vec3 tangentForce = vec3(0);
	vec3 movementForce = vec3(0);
	float coeff = 1.f;

	
	/*if (renderWindow->GetKeyPressed(GLFW_KEY_LEFT_SHIFT))
		coeff = 8.f;

	if (renderWindow->GetKeyPressed(GLFW_KEY_LEFT_CONTROL))
		coeff = 0.25f;*/

	if (renderWindow->GetKeyPressed('W'))
		tangentForce.z = 1.f;
	if (renderWindow->GetKeyPressed('S'))
		tangentForce.z = -1.f;

	if (renderWindow->GetKeyPressed('A'))
		tangentForce.x = 1.f;
	if (renderWindow->GetKeyPressed('D'))
		tangentForce.x = -1.f;

	if (renderWindow->GetKeyPressed('Q'))
		tangentForce.y = 1.f;
	if (renderWindow->GetKeyPressed('E'))
		tangentForce.y = -1.f;

	vec3 cameraForce = object->GetTransform().LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;
	transform.m_position += cameraForce;


	if (renderWindow->GetMousePressed(1))
	{
		double x, y;
		renderWindow->GetMouse(x ,y);

		vec3 deltaRotation = vec3(0);

		if (x - previousMouseInput.x > 0)
		{
			deltaRotation.y = 1.f;
		}
		else if (x - previousMouseInput.x < 0)
		{
			deltaRotation.y = -1.f;
		}
		if (y - previousMouseInput.y > 0)
		{
			deltaRotation.x = 1.f;
		}
		else if (y - previousMouseInput.y < 0)
		{
			deltaRotation.x = -1.f;
		}
		previousMouseInput.x = x;
		previousMouseInput.y = y;

		deltaRotation *= 6 * 0.01f;

		cameraRotation += deltaRotation;

		transform.SetRotationUsingEuler(cameraRotation);
	}
	
	object->SetTransform(transform);
}

bool EngineDemo::InitializeDemo(RenderWindow* _renderWindow)
{
	this->renderWindow = _renderWindow;
	sharedResources = new AssetManager();

	renderingManager = new RenderingManager(RenderingManager::Game);
	debugRenderingManager = new DebugRenderingManager();

	mainRenderer = new GL33Renderer();

	mainRenderer->InitializeRenderer(this->renderWindow, renderingManager);
	mainRenderer->m_assetManager = sharedResources;
	mainScene = new Scene();

	mainCamera = new Camera();
	{
		Transform camTransform;
		camTransform.m_position = vec3(10, -10, -15);
		camTransform.SetRotationUsingEuler(vec3(3.14 / 9, 0, 0));
		mainCamera->m_isControlEnabled = true;
	}


	mainRenderer->SetCamera(mainCamera);

	timer = new Time(10);

	mainScene = new Scene();

	GameObject* cameraObject = mainScene->CreateObject("Camera");
	cameraObject->AddComponent(mainCamera);

	mainScene->SetTimeObject(timer);

	sceneManager = new SceneManager(sharedResources);

	debug = new Debug(debugRenderingManager);

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	if (!mainRenderer->GetStatus())
	{
		printf("Failed to initiate Context!");
		return false;
	}

	mainScene->Initialize(renderingManager);

	Texture2D* blankDiff = TextureImport::LoadBMP("BlankTexture", "./resources/textures/blankDiffuse.bmp");
	this->sharedResources->SaveTexture(blankDiff);
	this->sharedResources->LoadTexture("BlankTexture");

	Texture2D* blankNorm = TextureImport::LoadBMP("BlankNormal", "./resources/textures/blankNormal.bmp");
	this->sharedResources->SaveTexture(blankNorm);
	this->sharedResources->LoadTexture("BlankNormal");

	Material* blankDiffusMat = new Material("BlankDiffuseMat");
	blankDiffusMat->AssignTexture("BlankTexture", "diffuseMap");
	blankDiffusMat->AssignTexture("BlankNormal", "normalMap");

	this->sharedResources->SaveMaterial(blankDiffusMat);
	this->sharedResources->LoadMaterial("BlankDiffuseMat");

	OBJImport obj;
	TriangleMesh skyMesh("SkyMesh");
	obj.ImportMesh("./resources/models/sphere.obj", skyMesh, true, true);
	this->sharedResources->SaveTriangleMesh(&skyMesh);
	this->sharedResources->LoadTriangleMesh("SkyMesh");

	Asset* skyMeshAsset = nullptr;
	this->sharedResources->GetAsset("SkyMesh", skyMeshAsset);

	TriangleMesh* sky = (TriangleMesh*)skyMeshAsset;

	GameObject* ball_obj = mainScene->CreateObject("sky");
	Transform t = ball_obj->GetTransform();
	t.m_scale = vec3(5000, 5000, 5000);
	ball_obj->SetTransform(t);
	MeshRenderer* meshRender = new MeshRenderer();
	ball_obj->AddComponent(meshRender);
	meshRender->AssignTriangleMesh(sky);
	meshRender->AssignMaterial(blankDiffusMat, 0);
	renderingManager->RegisterMeshRenderer(meshRender);
	
	DirectionalLightRender lr;
	cameraLight = new Camera();
	GameObject* bla = mainScene->CreateObject("ShadowCamera");
	t = bla->GetTransform();
	t.SetRotationUsingEuler(vec3(1, 0, 0));
	bla->SetTransform(t);
	bla->AddComponent(cameraLight);
	lr.m_shadowRender.m_shadowCamera.AttachCamera(cameraLight);
	lr.m_shadowRender.m_shadowCamera.SetOrthographicProj(-200, 200, -200, 200);
	lr.m_shadowRender.m_bufferSize = 8192;
	//mainRenderer->m_directionalLightsVector.push_back(lr);
	return true;
}

void EngineDemo::UpdateDemo()
{
	t += 0.0003;
	timer->Update();
	averageDet += mainScene->m_rigidBodySystem->m_collisionProcessor->m_detectionTime;
	averageProc += mainScene->m_rigidBodySystem->m_collisionProcessor->m_processingTime;
	averageContact += mainScene->m_rigidBodySystem->m_collisionProcessor->m_currentContacts.size();

	if (iteration > 10)
	{
		averageDet = 0;
		averageProc = 0;
		averageContact = 0;
		iteration = 0;
	}

	if (mainScene->m_enableSimulation)
		iteration++;
	double time = timer->GetTime();
	double deltaTime = timer->GetDelta();

	stringstream title;
	title << "BLAengine: " << timer->GetFramerate() << " fps, ";
	title << " ColDetTime: " << (averageDet / iteration);
	title << " ColProcTime: " << (averageProc / iteration);
	title << " ColProcTime: " << (averageContact / iteration);
	title.str();

#ifdef GLFW_INTERFACE
	if (renderWindow->GetKeyPressed(GLFW_KEY_SPACE) && (time - lastPressS) > 2)
	{
		cout << "Simulation ";
		if (mainScene->m_enableSimulation)
		{
			cout << "off\n";
			mainScene->m_enableSimulation = false;
		}
		else
		{
			cout << "on\n";
			mainScene->m_enableSimulation = true;
			stoped = false;
		}
		lastPressS = time;
	}

	if (renderWindow->GetKeyPressed(GLFW_KEY_LEFT_CONTROL) && (time - lastPressS) > 2)
	{
		moveLight = !moveLight;
		lastPressS = time;
	}

	GameObject* lightCam = mainScene->FindNameInScene("ShadowCamera");

	Transform lightT = lightCam->GetTransform();

	lightT.SetRotationUsingEuler(vec3(2*t, 0, 0));

	lightCam->SetTransform(lightT);

	if (renderWindow->GetKeyPressed(GLFW_KEY_BACKSPACE) && (time - lastPressG) > 0.5)
	{
		cout << "Gravity ";
		if (mainScene->GetGravity())
		{
			mainScene->DisableGravity();
			cout << "off\n";
		}
		else
		{
			cout << "on\n";
			mainScene->EnableGravity();
		}
		lastPressG = time;
	}
#endif

	if (renderWindow->GetMousePressed(0))
	{
		//ray = mainScene->ScreenToRay(1000);
		//debugRays.push_back(ray);
		vec3 colPoint(0);
		GameObject* object;// = cursorPicker->PickGameCharInScene(*mainScene, ray, colPoint);
	}

	if (mainScene->m_rigidBodySystem->m_collisionProcessor->debug_stop)
	{
		cout << "LCP Solver unhappy, Simulation Halted\n";
		mainScene->m_enableSimulation = false;
		mainScene->m_rigidBodySystem->m_collisionProcessor->debug_stop = false;
	}

	for (int c = 0; c < mainScene->GetContacts()->size(); c++)
	{
		Contact contact = mainScene->GetContacts()->at(c);

		debug->DrawRay(Ray(contact.m_contactPositionW, contact.m_contactNormalW, 1), vec3(0, 1, 0));
		debug->DrawRay(Ray(contact.m_contactPositionW, contact.m_contactTangent1W, 1), vec3(1, 0, 0));
		debug->DrawRay(Ray(contact.m_contactPositionW, contact.m_contactTangent2W, 1), vec3(0, 0, 1));

		//mainScene->m_enableSimulation = false;
	}

	debug->DrawGrid(1000, 10, vec3(0.4));

	debug->Update();

	if (GameObject* mainCamera = mainScene->FindNameInScene("Camera"))
	{
		SimpleControls(mainCamera, this->renderWindow);
		mainCamera->Update();
	}

	mainRenderer->Update();
	mainScene->Update();

	if (mainScene->m_enableSimulation)
		solvIt += (mainScene->m_rigidBodySystem->m_collisionProcessor->m_iterationCount);

	for (auto r : debugRays)
	{
		debug->DrawRay(r);
	}

	frameCount++;
#ifdef GLFW_INTERFACE
	if (renderWindow->GetKeyPressed(GLFW_KEY_ESCAPE))
	{
		terminationRequest = true;

		sceneManager->SaveScene("Test",mainScene);
	}
#endif
}
	
void EngineDemo::TerminateDemo()
{
	//renderWindow->~GLFWRenderWindow();
}