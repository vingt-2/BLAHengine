/*
This is a terribly awful quick testing pot-pourris of the features I put into the Engine,
It's not part of the final architecture but is merely there to test the new features as they come.
Hence the name "MainDemo.cpp".
*/

#define FULLSCREEN_SETTING 0

// Include standard headers
#include "./Common/std.h"
#include "./Common/Graphics.h"
#include "./Common/Maths.h"

#include "./Engine/Renderer/WINGL33_Renderer.h"

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

GameSingleton* gameSingleton;
GL33Renderer* mainRenderer;
SharedResources* sharedResources;
Debug* debug;
Scene* mainScene;
GameChar* currentObject = NULL;

vec3 cameraRotation = vec3(0);

double deltaTime = 0;


void GetFPS(int* fps_frames,GLfloat* fps_time,int* fps)
{
	GLfloat d_time;

	(*fps_frames) ++;

	d_time = glfwGetTime() - *fps_time;
	if (d_time > 1.0)
	{
		*fps = *fps_frames / (d_time);
		*fps_time = glfwGetTime();
		*fps_frames = 0;
	}
}

void LockFramerate(GLfloat hzFrequency)
{
	GLfloat deltaTime = glfwGetTime();
	while(glfwGetTime() - deltaTime < 1/hzFrequency)
	{
		// Lock
	}
}
void ObjectControl(GameChar* object)
{
	if (object == NULL)
		return;

	if ((glfwGetKey(mainRenderer->GetWindow(), 'F') == GLFW_PRESS))
	{
		object->m_rigidBody->AddTorque(vec3(0, 10, 0));
	}

	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_UP) == GLFW_PRESS))
	{
		object->m_rigidBody->AddLinearForce(vec3(0, 5, 0));
	}
	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS))
	{
		object->m_rigidBody->AddLinearForce(vec3(-5, 0, 0));
	}
	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS))
	{
		object->m_rigidBody->AddLinearForce(vec3(5, 0, 0));
	}
	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS))
	{
		object->m_rigidBody->AddLinearForce(vec3(0, -5, 0));
	}
}

void SimpleControls(GameChar* object)
{

	vec3 tangentForce = vec3(0);
	vec3 movementForce = vec3(0);
	float coeff = 1.f;


	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		coeff = 8.f;

	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS))
		coeff = 0.25f;

	if( (glfwGetKey(mainRenderer->GetWindow(), 'W'  ) == GLFW_PRESS) )
		tangentForce.z = 1.f;
	if( (glfwGetKey(mainRenderer->GetWindow(), 'S'  ) == GLFW_PRESS) )
		tangentForce.z = -1.f;

	if( (glfwGetKey(mainRenderer->GetWindow(), 'A'  ) == GLFW_PRESS) )
		tangentForce.x = 1.f;
	if( (glfwGetKey(mainRenderer->GetWindow(), 'D'  ) == GLFW_PRESS) )
		tangentForce.x = -1.f;

	if ((glfwGetKey(mainRenderer->GetWindow(), 'Q') == GLFW_PRESS))
		tangentForce.y = 1.f;
	if ((glfwGetKey(mainRenderer->GetWindow(), 'E') == GLFW_PRESS))
		tangentForce.y = -1.f;

	vec3 cameraForce = object->m_transform->LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;
	cameraForce *= 5*deltaTime;
	object->m_transform->m_position += cameraForce;


	if( (glfwGetMouseButton(mainRenderer->GetWindow(), 1 ) == GLFW_PRESS) )
	{
		double x, y;
		glfwGetCursorPos(mainRenderer->GetWindow(),&x,&y);

		vec3 deltaRotation = vec3(0);

		if(x - previousMouseInput->x > 0)
		{
			deltaRotation.y = 1.f;
		}
		else if(x - previousMouseInput->x < 0)
		{
			deltaRotation.y = -1.f;
		}
		if(y - previousMouseInput->y > 0)
		{
			deltaRotation.x = 1.f;
		}
		else if(y - previousMouseInput->y < 0)
		{
			deltaRotation.x = -1.f;
		}
		previousMouseInput->x = x;
		previousMouseInput->y = y;

		deltaRotation *= 6*deltaTime;

		cameraRotation += deltaRotation;

		object->m_transform->SetRotationUsingEuler(cameraRotation);
	}
	else
	{

	}
}

void Idle(int* fps_frames,GLfloat* fps_time,int* fps)
{
	GetFPS(fps_frames,fps_time, fps);
	//LockFramerate(100.f); // Lock framerate 100 hz
}

void SetObject(GameChar* object)
{
	object->m_transform->m_position = (vec3(-10, 0, 0));

	object->m_transform->SetRotationUsingEuler(vec3(1, 1, 1));

	object->m_rigidBody->m_angularVelocity = vec3(0);
	object->m_rigidBody->m_velocity = vec3(0);

	//object->m_transform->m_scale = vec3(0.1);
}


int main( void )
{
	sharedResources = new SharedResources();
	
	int mainRenderFullScreen    = FULLSCREEN_SETTING;

	mainRenderer				= new GL33Renderer((char*)"BLAengine - MainDemo",mainRenderFullScreen);

	gameSingleton               = new GameSingleton(mainRenderer,sharedResources);

	Camera* mainCamera = new Camera();
	mainCamera->m_transform->m_position = vec3(10, 10, -15);
	mainCamera->m_transform->SetRotationUsingEuler(vec3(3.14 / 9, 0, 0));
	mainCamera->m_isControlEnabled = true;

	mainRenderer->SetCamera(mainCamera);
	
	mainScene = new Scene(mainCamera);

	RenderingManager* renderingManager = new RenderingManager(1, mainRenderer, RenderingManager::Game);
	RenderingManager* debugRenderingManager = new RenderingManager(1, mainRenderer, RenderingManager::DebugGizmo);

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

	sharedResources->LoadMaterial("DirLightPass", "./resources/shaders/Lighting/DirectLightVS.glsl", "./resources/shaders/Lighting/DirectLightFS.glsl");

	mainRenderer->DrawColorBufferPrgmID = sharedResources->GetMaterial("DrawColorTexture");
	mainRenderer->DrawDepthBufferPrgmID = sharedResources->GetMaterial("DrawDepthTexture");

	mainRenderer->m_GBuffer.m_geometryPassPrgmID = sharedResources->GetMaterial("GeomPass");

	sharedResources->loadBMP_custom("testDiffuse","./resources/textures/damier.bmp");
	sharedResources->loadBMP_custom("blankDiffuse", "./resources/textures/blank.bmp");
	sharedResources->loadBMP_custom("earthDiffuse","./resources/textures/earth.bmp");
	sharedResources->loadBMP_custom("earthNormals","./resources/textures/earth_NRM.bmp");
	sharedResources->loadBMP_custom("red", "./resources/textures/red.bmp");

	//mainRenderer->m_debug = debug;
	OBJImport objImport;
	TriangleMesh xwing;
	TriangleMesh sphere;
	TriangleMesh invertedSphere;
	TriangleMesh floor;
	TriangleMesh cube;
	TriangleMesh sponza;
	TriangleMesh arrow;

	//objImport.ImportMesh("./resources/models/x-wing.obj", &xwing, false);
	objImport.ImportMesh("./resources/models/sponza.obj", sponza, false);
	objImport.ImportMesh("./resources/models/cube.obj", cube, false);
	objImport.ImportMesh("./resources/models/cube.obj", floor, false);
	objImport.ImportMesh("./resources/models/sphere.obj", sphere, false);
	objImport.ImportMesh("./resources/models/sphere.obj", invertedSphere, true);
	sponza.ApplyGeomScaling(vec3(0.1f));
	floor.ApplyGeomScaling(vec3(100, 10, 100));
	floor.ApplyUVScaling(vec2(100));
	
	//GameChar* floor_obj = new GameChar();
	//floor_obj->SetTriangleMesh(&floor);
	//floor_obj->m_meshRenderer->AssignMaterial("defaultShader");
	//floor_obj->m_meshRenderer->AssignTexture("blankDiffuse", "texture");
	//floor_obj->m_meshRenderer->AssignTexture("blankDiffuse", "normals");
	//renderingManager->RequestRenderTicket(*floor_obj);
	//mainScene->AddObject(floor_obj);
	//floor_obj->m_transform->m_position = (vec3(0, 0, 0));
	//floor_obj->m_rigidBody->m_isPinned = true;

	GameChar* skySphere = new GameChar();
	skySphere->SetTriangleMesh(&invertedSphere);
	skySphere->m_meshRenderer->AssignMaterial("DirLightPass");
	skySphere->m_meshRenderer->AssignTexture("blankDiffuse", "texture");
	skySphere->m_meshRenderer->AssignTexture("blankDiffuse", "normals");
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
	//sceneMesh->m_meshRenderer->AssignTexture("blankDiffuse", "normals");
	//sceneMesh->m_transform->m_scale = vec3(0.1);
	//renderingManager->RequestRenderTicket(*sceneMesh);
	//mainScene->AddObject(sceneMesh);
	//sceneMesh->m_rigidBody->m_isPinned = true;

	GameChar* Ball = new GameChar();
	Ball->SetTriangleMesh(&sphere);
	Ball->m_meshRenderer->AssignMaterial("DirLightPass");
	Ball->m_meshRenderer->AssignTexture("earthDiffuse", "diffuseMap");
	Ball->m_meshRenderer->AssignTexture("earthNormals", "normalMap");
	renderingManager->RequestRenderTicket(*Ball);
	mainScene->AddObject(Ball);
	Ball->m_rigidBody->m_isPinned = true;

	GameChar* lightObj = new GameChar();
	lightObj->SetTriangleMesh(&cube);
	lightObj->m_meshRenderer->AssignMaterial("DirLightPass");
	lightObj->m_meshRenderer->AssignTexture("blankDiffuse", "diffuseMap");
	lightObj->m_meshRenderer->AssignTexture("blankDiffuse", "normals");
	lightObj->m_transform->m_position = vec3(-10,5,0);
	lightObj->m_transform->SetRotationUsingEuler(vec3(-1, 0, 0));

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

	cout << "setuping buffer " << mainRenderer->SetupDirectionalShadowBuffer(lr.m_shadowRender) << "\n";;
	mainRenderer->m_directionalLightsVector.push_back(lr);

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();
	
	Ray ray(vec3(0),vec3(0),0);


	cout << "\n\n\n\nDone Loading, RigidBody Simulation: \n";

	//
	// Did you know ?
	// There is a big memory leak when you draw debug rays.
	// So don't do it too much.
	// Cheers
	// 

	int frameCount = 0;
	vector<Ray> debugRays;
	double lastPressG = 0;
	double lastPressS = 0;
	double lastPressR = 0;
	double oldTime = glfwGetTime();

	mainScene->DisableGravity();

	bool stoped = false;
	int C = 0;

	vec3 currentRay(0);
	vec3 currentPoint(0);

	double averageDet = 0;
	double averageProc = 0;

	double iteration = 1;

	int solvIt = 0;
	float lightRotation = 0;
	bool moveLight = false;
	while(!terminationRequest)
	{
		averageDet += mainScene->m_rigidBodySystem->m_collisionProcessor->m_detectionTime;
		averageProc += mainScene->m_rigidBodySystem->m_collisionProcessor->m_processingTime;

		if(mainScene->m_enableSimulation)
			iteration++;
		double time = glfwGetTime();
		deltaTime = time - oldTime;
		oldTime = time;
		Idle(&fps_frames,&fps_time,&fps); 

		stringstream title;
		title << "BLAengine: " << fps << " fps, ";
		title << " ColDetTime: " << (averageDet / iteration);
		title << " ColProcTime: " << (averageProc / iteration);
		title.str();
		glfwSetWindowTitle(mainRenderer->GetWindow(), title.str().data());

		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) && (time - lastPressS) > 2)
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

		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) && (time - lastPressS) > 2)
		{
			lightObj->m_transform->m_position = mainCamera->m_transform->m_position;
			lightObj->m_transform->m_rotation = mainCamera->m_transform->m_rotation;
			moveLight = !moveLight;
			lastPressS = time;
		}

		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS))
		{
			lightRotation += 0.1*deltaTime;
		}

		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS))
		{
			lightRotation -= 0.1*deltaTime;
		}

		if (moveLight)
		{
			lightRotation += 0.1*deltaTime;
		}

		lightObj->m_transform->SetRotationUsingEuler(vec3(lightRotation, 0, 0));


		if (glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS && (time - lastPressG) > 0.5)
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
		
		if(glfwGetMouseButton(mainRenderer->GetWindow(), 0))
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
				renderingManager->DebugDrawRedSphere(colPoint);
				currentObject->m_rigidBody->PushForceWorld(colPoint, ray.m_direction);
			}
		}


		if (glfwGetMouseButton(mainRenderer->GetWindow(), 2))
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

		for (int c = 0; c < mainScene->GetContacts()->size(); c++)
		{
			Contact contact = mainScene->GetContacts()->at(c);

			renderingManager->DebugDrawRedSphere(contact.m_contactPositionW);
			debug->DrawRay(contact.m_contactPositionW, contact.m_contactNormalW, 1);
			debug->DrawRay(contact.m_contactPositionW, contact.m_contactTangent1W, 1);
			debug->DrawRay(contact.m_contactPositionW, contact.m_contactTangent2W, 1);

//			mainScene->m_enableSimulation = false;
		}

		if (mainScene->m_rigidBodySystem->m_collisionProcessor->debug_stop)
		{
			cout << "LCP Solver unhappy, Simulation Halted\n";
			mainScene->m_enableSimulation = false;
			mainScene->m_rigidBodySystem->m_collisionProcessor->debug_stop = false;
		}


		SimpleControls(mainCamera);
		
		mainCamera->Update();
		mainRenderer->Update();
		mainScene->Update();

		if(mainScene->m_enableSimulation)
			solvIt += (mainScene->m_rigidBodySystem->m_collisionProcessor->m_iterationCount);



		frameCount++;
		if( (glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_ESCAPE ) == GLFW_PRESS)  |  glfwWindowShouldClose(mainRenderer->GetWindow()) )
		{
			terminationRequest = true;
		}

		cameraLight->UpdateView();
	}


	glfwTerminate();

	return 0;
}
