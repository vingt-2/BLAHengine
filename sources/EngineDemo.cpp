#include "EngineDemo.h"
using namespace BLAengine;

vec2 previousMouseInput;
vec3 cameraRotation;

void SetObject(GameChar* object)
{
	object->m_transform->m_position = (vec3(-10, 0, 0));

	object->m_transform->SetRotationUsingEuler(vec3(1, 1, 1));

	object->m_rigidBody->m_angularVelocity = vec3(0);
	object->m_rigidBody->m_velocity = vec3(0);

	//object->m_transform->m_scale = vec3(0.1);
}

void SimpleControls(GameChar* object, RenderWindow* renderWindow)
{

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

	vec3 cameraForce = object->m_transform->LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;
	cameraForce *= 10 * 0.01f;
	object->m_transform->m_position += cameraForce;


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

		object->m_transform->SetRotationUsingEuler(cameraRotation);
	}
	else
	{

	}
}

bool EngineDemo::InitializeDemo(RenderWindow* _renderWindow)
{
	this->renderWindow = _renderWindow;
	sharedResources = new AssetManager();

	mainRenderer = new GL33Renderer();

	mainRenderer->InitializeRenderer(this->renderWindow);

	gameSingleton = new GameSingleton(mainRenderer, sharedResources);

	mainCamera = new Camera();
	mainCamera->m_transform->m_position = vec3(10, -10, -15);
	mainCamera->m_transform->SetRotationUsingEuler(vec3(3.14 / 9, 0, 0));
	mainCamera->m_isControlEnabled = true;

	mainRenderer->SetCamera(mainCamera);

	timer = new Time(10);

	mainScene = new Scene();

	mainScene->SetTimeObject(timer);

	mainScene->AddObject(mainCamera);

	sceneManager = new SceneManager();

	renderingManager = new RenderingManager(mainRenderer, RenderingManager::Game);
	debugRenderingManager = new DebugRenderingManager(mainRenderer);

	debug = new Debug(debugRenderingManager);

	cursorPicker = new CursorPicker(gameSingleton);

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	if (!mainRenderer->GetStatus())
	{
		printf("Failed to initiate Context!");
		return false;
	}

	TriangleMesh* floor = new TriangleMesh(string("floor"));

	GameChar* floor_obj = new GameChar();
	floor_obj->m_objectName = string("Jean-Robert");
	floor_obj->SetTriangleMesh(floor);
	//renderingManager->RequestRenderTicket(*floor_obj);
	floor_obj->m_rigidBody->SetCollider(new Collider(floor));
	floor_obj->m_transform->m_scale = vec3(1);
	mainScene->AddObject(floor_obj);
	floor_obj->m_transform->m_position = (vec3(0, -5, 0));
	floor_obj->m_rigidBody->m_isPinned = true;

	DirectionalLight* dirLight = new DirectionalLight(vec3(1, 1, 1));
	mainScene->AddDirectionalLight(dirLight);
	sceneManager->SaveScene("myScene",mainScene);
	sceneManager->LoadScene("myScene");
	return true;
}

void EngineDemo::UpdateDemo()
{
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
		ray = cursorPicker->ScreenToRay(1000);
		//debugRays.push_back(ray);
		vec3 colPoint(0);
		GameChar* object = cursorPicker->PickGameCharInScene(*mainScene, ray, colPoint);

		if (currentObject== nullptr || currentObject != object)
		{
			currentObject = object;
		}
		else
		{
			currentObject->m_rigidBody->PushForceWorld(colPoint, ray.m_direction);
		}
	}

	if (renderWindow->GetMousePressed(2))
	{
		ray = cursorPicker->ScreenToRay(1000);
		//debugRays.push_back(ray);
		vec3 colPoint(0);
		GameChar* object = cursorPicker->PickGameCharInScene(*mainScene, ray, colPoint);
		if (object != nullptr)
		{
			object->m_rigidBody->m_velocity = vec3(0);
			object->m_rigidBody->m_angularVelocity = vec3(0);
		}
	}

	if (currentObject != nullptr && currentRay != vec3(0))
	{
		vec3 a = currentObject->m_transform->LocalPositionToWorld(currentPoint);
		vec3 b = currentObject->m_transform->LocalDirectionToWorld(currentRay);
		currentObject->m_rigidBody->PushForceWorld(a, 0.5f*b);
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
	//SimpleControls(mainCamera, this->renderWindow);
	mainCamera->Update();
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
	}
#endif
}
	
void EngineDemo::TerminateDemo()
{
	//renderWindow->~GLFWRenderWindow();
}