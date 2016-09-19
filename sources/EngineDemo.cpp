/*
This is a terribly awful quick testing pot-pourris of the features I put into the Engine,
It's not part of the final architecture but is merely there to test the new features as they come.
Hence the name "MainDemo.cpp".
*/

// Include standard headers
#include "./Common/StdInclude.h"
#include "./Common/System.h"
#ifdef BLA_NO_DLL

#define FULLSCREEN_SETTING 0

#include "./Common/Maths.h"

#include "./Engine/Renderer/GL33Renderer.h"

#include "AssetsImport\OBJImport.h"
#include "Engine\Game\RenderingManager.h"
#include "Engine\Game\Debug.h"
#include "./Engine/Game/Scene.h"
#include "./Engine/Game/CursorPicker.h"
#include "./Engine/Game/GameSingleton.h"
#include "./Engine/Game/GameComponents/Collider.h"
#include "./Engine/Game/RigidBodySystem.h"

int fps = 60;
vec2* previousMouseInput = new vec2(0,0);

using namespace BLAengine;

GameSingleton* gameSingleton;
GL33Renderer* mainRenderer;
SharedResources* sharedResources;
Debug* debug;
Scene* mainScene;
GameChar* currentObject = NULL;
vec3 cameraRotation = vec3(0);
GLFWRenderWindow* renderWindow;

void SetObject(GameChar* object)
{
	object->m_transform->m_position = (vec3(-10, 0, 0));

	object->m_transform->SetRotationUsingEuler(vec3(1, 1, 1));

	object->m_rigidBody->m_angularVelocity = vec3(0);
	object->m_rigidBody->m_velocity = vec3(0);

	//object->m_transform->m_scale = vec3(0.1);
}

void SimpleControls(GameChar* object)
{

	vec3 tangentForce = vec3(0);
	vec3 movementForce = vec3(0);
	float coeff = 1.f;

	
	if (renderWindow->GetKeyPressed(GLFW_KEY_LEFT_SHIFT))
		coeff = 8.f;

	if (renderWindow->GetKeyPressed(GLFW_KEY_LEFT_CONTROL))
		coeff = 0.25f;

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
	cameraForce *= 5 * 0.01f;
	object->m_transform->m_position += cameraForce;


	if (renderWindow->GetMousePressed(1))
	{
		double x, y;
		renderWindow->GetMouse(x ,y);

		vec3 deltaRotation = vec3(0);

		if (x - previousMouseInput->x > 0)
		{
			deltaRotation.y = 1.f;
		}
		else if (x - previousMouseInput->x < 0)
		{
			deltaRotation.y = -1.f;
		}
		if (y - previousMouseInput->y > 0)
		{
			deltaRotation.x = 1.f;
		}
		else if (y - previousMouseInput->y < 0)
		{
			deltaRotation.x = -1.f;
		}
		previousMouseInput->x = x;
		previousMouseInput->y = y;

		deltaRotation *= 6 * 0.01f;

		cameraRotation += deltaRotation;

		object->m_transform->SetRotationUsingEuler(cameraRotation);
	}
	else
	{

	}
}

int main( void )
{
	sharedResources = new SharedResources();
	int mainRenderFullScreen    = FULLSCREEN_SETTING;

	renderWindow = new GLFWRenderWindow();
	renderWindow->CreateWindow("BLAengine", 100, 100, mainRenderFullScreen);

	mainRenderer = new GL33Renderer();

	mainRenderer->InitializeRenderer((RenderWindow*) renderWindow);

	gameSingleton               = new GameSingleton(mainRenderer,sharedResources);

	Camera* mainCamera = new Camera();
	mainCamera->m_transform->m_position = vec3(10, -5, -15);
	mainCamera->m_transform->SetRotationUsingEuler(vec3(3.14 / 9, 0, 0));
	mainCamera->m_isControlEnabled = true;

	mainRenderer->SetCamera(mainCamera);

	Time* timer = new Time(10);

	mainScene = new Scene(timer, mainCamera);

	RenderingManager* renderingManager = new RenderingManager(mainRenderer, RenderingManager::Game);
	DebugRenderingManager* debugRenderingManager = new DebugRenderingManager(mainRenderer);

	debug = new Debug(debugRenderingManager);

	CursorPicker cursorPicker(gameSingleton);

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	if(!mainRenderer->GetStatus())
	{
		printf("Failed to initiate Context!");
		return -1;
	}

	// NOW WE CAN LOAD SOME RESSOURCES
	sharedResources->LoadMaterial("ShadowmapPass", "./resources/shaders/Engine/ShadowMapVS.glsl", "./resources/shaders/Engine/ShadowMapFS.glsl");
	sharedResources->LoadMaterial("GeomPass", "./resources/shaders/Engine/GeomPassVS.glsl", "./resources/shaders/Engine/GeomPassFS.glsl");
	sharedResources->LoadMaterial("DrawDepthTexture", "./resources/shaders/Engine/DrawDepthTextureVS.glsl", "./resources/shaders/Engine/DrawDepthTextureFS.glsl");
	sharedResources->LoadMaterial("DrawColorTexture", "./resources/shaders/Engine/DrawColorTextureVS.glsl", "./resources/shaders/Engine/DrawColorTextureFS.glsl");
	sharedResources->LoadMaterial("DebugRaysForward", "./resources/shaders/Engine/DebugRaysShaderVS.glsl", "./resources/shaders/Engine/DebugRaysShaderFS.glsl");
	sharedResources->LoadMaterial("PointLightStencil", "./resources/shaders/Engine/PointLightStencilVS.glsl", "./resources/shaders/Engine/PointLightStencilFS.glsl");
	sharedResources->LoadMaterial("DirLightPass", "./resources/shaders/Lighting/DirectLightVS.glsl", "./resources/shaders/Lighting/DirectLightFS.glsl");
	sharedResources->LoadMaterial("PointLightPass", "./resources/shaders/Lighting/PointLightVS.glsl", "./resources/shaders/Lighting/PointLightFS.glsl");

	mainRenderer->DrawColorBufferPrgmID = sharedResources->GetMaterial("DrawColorTexture");
	mainRenderer->DrawDepthBufferPrgmID = sharedResources->GetMaterial("DrawDepthTexture");
	mainRenderer->m_GBuffer.m_geometryPassPrgmID = sharedResources->GetMaterial("GeomPass");
	mainRenderer->m_GBuffer.m_drawSphereStencilPgrmID = sharedResources->GetMaterial("PointLightStencil");
	mainRenderer->m_debugRayPgrmID = sharedResources->GetMaterial("DebugRaysForward");


	sharedResources->loadBMP_custom("testDiffuse","./resources/textures/damier.bmp");
	sharedResources->loadBMP_custom("blankDiffuse", "./resources/textures/blankDiffuse.bmp");
	sharedResources->loadBMP_custom("blankNormal", "./resources/textures/blankNormal.bmp");
	sharedResources->loadBMP_custom("earthDiffuse","./resources/textures/earth.bmp");
	sharedResources->loadBMP_custom("earthNormals","./resources/textures/earth_NRM.bmp");
	sharedResources->loadBMP_custom("red", "./resources/textures/red.bmp");

	mainRenderer->m_sharedResources = sharedResources;

	//mainRenderer->m_debug = debug;
	OBJImport objImport;
	TriangleMesh xwing;
	TriangleMesh sphere;
	TriangleMesh invertedSphere;
	TriangleMesh floor;
	TriangleMesh cube;
	TriangleMesh sponza;
	TriangleMesh arrow;
	TriangleMesh ball;

	//objImport.ImportMesh("./resources/models/x-wing.obj", &xwing, false, false);
	//objImport.ImportMesh("./resources/models/sponza.obj", sponza, false, false);
	objImport.ImportMesh("./resources/models/cube.obj", cube, false, false);
	objImport.ImportMesh("./resources/models/cube.obj", floor, false, false);
	objImport.ImportMesh("./resources/models/sphere.obj", sphere, false, true);
	objImport.ImportMesh("./resources/models/sphere.obj", ball, false, false);
	objImport.ImportMesh("./resources/models/sphere.obj", invertedSphere, true, false);
	//sponza.ApplyGeomScaling(vec3(0.1));
	floor.ApplyGeomScaling(vec3(100, 10, 100));
	floor.ApplyUVScaling(vec2(100));
	
	GameChar* floor_obj = new GameChar();
	floor_obj->SetTriangleMesh(&floor);
	floor_obj->m_meshRenderer->AssignMaterial("defaultShader");
	floor_obj->m_meshRenderer->AssignTexture("blankDiffuse", "texture");
	floor_obj->m_meshRenderer->AssignTexture("blankNormal", "normals");
	renderingManager->RequestRenderTicket(*floor_obj);
	floor_obj->m_rigidBody->SetCollider(new Collider(&floor));
	floor_obj->m_transform->m_scale = vec3(1);
	mainScene->AddObject(floor_obj);
	floor_obj->m_transform->m_position = (vec3(0, -5, 0));
	floor_obj->m_rigidBody->m_isPinned = true;

	GameChar* skySphere = new GameChar();
	skySphere->SetTriangleMesh(&invertedSphere);
	skySphere->m_meshRenderer->AssignMaterial("DirLightPass");
	skySphere->m_meshRenderer->AssignTexture("blankDiffuse", "texture");
	skySphere->m_meshRenderer->AssignTexture("blankNormal", "normals");
	renderingManager->RequestRenderTicket(*skySphere);
	mainScene->AddObject(skySphere);
	skySphere->m_transform->m_position = (vec3(0, 0, 0));
	skySphere->m_transform->m_scale = vec3(1000);
	skySphere->m_rigidBody->m_isPinned = true;
	skySphere->m_rigidBody->m_enableCollision = false;

	//GameChar* sceneMesh = new GameChar();
	//sceneMesh->SetTriangleMesh(&sponza);
	//sceneMesh->m_meshRenderer->AssignMaterial("DirLightPass");
	//sceneMesh->m_meshRenderer->AssignTexture("blankDiffuse", "diffuseMap");
	//sceneMesh->m_meshRenderer->AssignTexture("blankNormal", "normals");
	//renderingManager->RequestRenderTicket(*sceneMesh);
	//mainScene->AddObject(sceneMesh);
	//sceneMesh->m_rigidBody->m_isPinned = true;

	for (int i = 2; i < 10; i++)
	{
		GameChar* Ball = new GameChar();
		Ball->SetTriangleMesh(&ball);
		Ball->m_meshRenderer->AssignMaterial("DirLightPass");
		Ball->m_meshRenderer->AssignTexture("earthDiffuse", "diffuseMap");
		Ball->m_meshRenderer->AssignTexture("earthNormals", "normalMap");
		renderingManager->RequestRenderTicket(*Ball);
		Ball->m_rigidBody->SetCollider(new Collider(&ball));
		mainScene->AddObject(Ball);
		Ball->m_rigidBody->m_isPinned = false;
		Ball->m_transform->m_position = vec3(0,3 + 20 * i, 0);
	}

	GameChar* lightObj = new GameChar();
	lightObj->SetTriangleMesh(&cube);
	lightObj->m_meshRenderer->AssignMaterial("DirLightPass");
	lightObj->m_meshRenderer->AssignTexture("blankDiffuse", "diffuseMap");
	lightObj->m_meshRenderer->AssignTexture("blankDiffuse", "normals");
	lightObj->m_transform->m_position = vec3(-10,30,0);
	lightObj->m_transform->SetRotationUsingEuler(vec3(-1, -1, 0));

	Camera* cameraLight = new Camera();
	cameraLight->m_transform = lightObj->m_transform;
	lightObj->m_transform->m_position = mainCamera->m_transform->m_position;
	lightObj->m_transform->m_rotation = mainCamera->m_transform->m_rotation;


	DirectionalLight* light = new DirectionalLight(vec3(0,-1,0));
	mainScene->AddDirectionalLight(light);

	DirectionalLightRender lr;
	lr.m_lightRenderPrgmID = sharedResources->GetMaterial("DirLightPass");
	lr.m_shadowRender.m_shadowPrgmID = sharedResources->GetMaterial("ShadowmapPass");
	lr.m_shadowRender.m_shadowCamera.AttachCamera(cameraLight);
	lr.m_shadowRender.m_shadowCamera.SetOrthographicProj(-200, 200, -200, 200);
	lr.m_shadowRender.m_bufferSize = 8192;

	RenderData* sphereRenderData = sphere.GenerateRenderData();
	mainRenderer->SetupPointLightRenderSphere(sphereRenderData->m_vertPos, sphereRenderData->m_triangleIndices);

	cout << "setuping buffer " << mainRenderer->SetupDirectionalShadowBuffer(lr.m_shadowRender) << "\n";;
	mainRenderer->m_directionalLightsVector.push_back(lr);
	
	Ray ray(vec3(0),vec3(0),0);

	cout << "\n\n\n\nDone Loading, RigidBody Simulation: \n";

	int frameCount = 0;
	vector<Ray> debugRays;
	double lastPressG = 0;
	double lastPressS = 0;
	double lastPressR = 0;

	mainScene->DisableGravity();

	bool stoped = false;
	int C = 0;

	vec3 currentRay(0);
	vec3 currentPoint(0);

	double averageDet = 0;
	double averageProc = 0;
	float averageContact = 0;

	double iteration = 1;

	int solvIt = 0;
	float lightRotation = 1;
	bool moveLight = false;

	debug->m_drawDebugRays = true;

	for (int i = 0; i < 100; i++)
	{
		PointLightRender pointLight;
		pointLight.m_transform = new Transform();
		pointLight.m_transform->m_scale = vec3(30);
		pointLight.m_transform->m_position = vec3(i*5,10+(2*i),0);
		pointLight.m_transform->UpdateTransform();
		pointLight.m_pointLightPrgmID = sharedResources->GetMaterial("PointLightPass");
		mainRenderer->m_pointLightsVector.push_back(pointLight);
		debugRays.push_back(Ray(pointLight.m_transform->m_position, pointLight.m_transform->LocalDirectionToWorld(vec3(1, 0, 0)), 1));
		debugRays.push_back(Ray(pointLight.m_transform->m_position, pointLight.m_transform->LocalDirectionToWorld(vec3(0, 0, 1)), 1));
		debugRays.push_back(Ray(pointLight.m_transform->m_position, pointLight.m_transform->LocalDirectionToWorld(vec3(0, 1, 0)), 1));
	}
	
	while(!terminationRequest)
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

		if(mainScene->m_enableSimulation)
			iteration++;
		double time = timer->GetTime();
		double deltaTime = timer->GetDelta();

		stringstream title;
		title << "BLAengine: " << timer->GetFramerate() << " fps, ";
		title << " ColDetTime: " << (averageDet / iteration);
		title << " ColProcTime: " << (averageProc / iteration);
		title << " ColProcTime: " << (averageContact / iteration);
		title.str();

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
			lightObj->m_transform->m_position = mainCamera->m_transform->m_position;
			lightObj->m_transform->m_rotation = mainCamera->m_transform->m_rotation;
			moveLight = !moveLight;
			lastPressS = time;
		}

		if (renderWindow->GetKeyPressed(GLFW_KEY_LEFT))
		{
			lightRotation += 0.5*deltaTime;
		}

		if (renderWindow->GetKeyPressed(GLFW_KEY_RIGHT))
		{
			lightRotation -= 0.5*deltaTime;
		}

		if (moveLight)
		{
			lightRotation += 0.1*deltaTime;
		}

		lightObj->m_transform->SetRotationUsingEuler(vec3(lightRotation, 0, 0));

		
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

		//cout << collider_1->CollidesWith(collider_2);
		
		if(renderWindow->GetMousePressed(0))
		{
			ray = cursorPicker.ScreenToRay(1000);
			//debugRays.push_back(ray);
			vec3 colPoint(0);
			GameChar* object = cursorPicker.PickGameCharInScene(*mainScene, ray, colPoint);

			if (currentObject == NULL || currentObject != object)
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
			ray = cursorPicker.ScreenToRay(1000);
			//debugRays.push_back(ray);
			vec3 colPoint(0);
			GameChar* object = cursorPicker.PickGameCharInScene(*mainScene, ray, colPoint);
			if (object != NULL)
			{
				object->m_rigidBody->m_velocity = vec3(0);
				object->m_rigidBody->m_angularVelocity = vec3(0);
			}
		}

		if (currentObject != NULL && currentRay != vec3(0))
		{
			vec3 a = currentObject->m_transform->LocalPositionToWorld(currentPoint);
			vec3 b = currentObject->m_transform->LocalDirectionToWorld(currentRay);
			currentObject->m_rigidBody->PushForceWorld(a,0.5f*b);
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
		SimpleControls(mainCamera);
		mainCamera->Update();
		mainRenderer->Update();
		mainScene->Update();

		if(mainScene->m_enableSimulation)
			solvIt += (mainScene->m_rigidBodySystem->m_collisionProcessor->m_iterationCount);

		for (auto r : debugRays)
		{
			debug->DrawRay(r);
		}

		frameCount++; 
		if(renderWindow->GetKeyPressed(GLFW_KEY_ESCAPE))
		{
			terminationRequest = true;
		}

		cameraLight->UpdateView();
	}


	glfwTerminate();

	return 0;
}
#endif
