/*
This is a terribly awful quick testing pot-pourris of the features I put into the Engine,
It's not part of the final architecture but is merely there to test the new features as they come.
Hence the name "MainDemo.cpp".
*/

#define FULLSCREEN_SETTING 0

// Include standard headers
#include "./Std/std.h"
#include "./std/Graphics.h"

#ifdef USE_OSXGL32_RENDERER
#include "./Engine/OSXGL32_Renderer.h"
#define RENDERER 0
#endif

#ifdef USE_WINGL33_RENDERER
#include "./Engine/Renderer/WINGL33_Renderer.h"
#define RENDERER 1
#endif

#include "Engine\Game\RenderingManager.h"
#include "Engine\Game\Debug.h"
#include "./Engine/Game/Scene.h"
#include "./Engine/Game/CursorPicker.h"
#include "./Engine/Game/GameSingleton.h"
#include "./Engine/Game/GameComponents/Collider.h"
#include "./Engine/Game/RigidBodySystem.h"
#include "./Engine/AssetsManager/MeshAsset.h"

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
	mainCamera->m_transform->m_position = vec3(0, -10, -15);
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
	sharedResources->LoadMaterial("defaultShader","./resources/shaders/Vertex_Shader.glsl", "./resources/shaders/Fragment_Shader.glsl");
	sharedResources->LoadMaterial("debugShader", "./resources/shaders/Debug_Vertex.glsl", "./resources/shaders/Debug_Fragment.glsl");
	sharedResources->LoadMaterial("shadowmapShader", "./resources/shaders/Vert_Shadow.glsl", "./resources/shaders/Frag_Shadow.glsl");
	sharedResources->LoadMaterial("depthBufDebug", "./resources/shaders/depthBuffDebug_vert.glsl", "./resources/shaders/depthBuffDebug_frag.glsl");

	mainRenderer->SetShadowID(sharedResources->GetMaterial("shadowmapShader"));

	mainRenderer->depthBufDebugPrgm = sharedResources->GetMaterial("depthBufDebug");
	
	sharedResources->loadBMP_custom("testDiffuse","./resources/textures/damier.bmp");
	sharedResources->loadBMP_custom("blankDiffuse", "./resources/textures/blank.bmp");
	sharedResources->loadBMP_custom("earthDiffuse","./resources/textures/earth.bmp");
	sharedResources->loadBMP_custom("earthNormals","./resources/textures/earth_NRM.bmp");
	sharedResources->loadBMP_custom("red", "./resources/textures/red.bmp");

	//mainRenderer->m_debug = debug;
	OBJImport objImport;

	MeshAsset xwing;
	MeshAsset sphere;
	MeshAsset floor;
	MeshAsset cube;
	MeshAsset rocket;
	//objImport.ImportMesh("./resources/models/x-wing.obj", &xwing, false);
	objImport.ImportMesh("./resources/models/cube.obj", &cube, false);
	objImport.ImportMesh("./resources/models/cube.obj", &floor, false);
	objImport.ImportMesh("./resources/models/bla.obj", &sphere, false);
	objImport.ImportMesh("./resources/models/Y8490_Rocket.obj", &rocket, true);
	mat4 scaleMat(vec4(100, 0, 0, 0), 
					vec4(0, 10, 0, 0), 
					vec4(0, 0, 100, 0),
					vec4(0, 0, 0, 1));
	for (auto &v : floor.m_meshVertices){
		vec4 hP = scaleMat * vec4(v,1);
		v = vec3(hP.x, hP.y, hP.z);
	}
	for (auto &v : floor.m_meshUVs){
		v = mat2(100) * v;
	}

	GameChar* floor_obj = new GameChar(&floor);

	floor_obj->m_meshRenderer->AssignMaterial("defaultShader");
	
	floor_obj->m_meshRenderer->AssignTexture("testDiffuse","texture");
	floor_obj->m_meshRenderer->AssignTexture("earthNormals","normals");

	GameChar* debugSphere = new GameChar(&cube);
	debugSphere->m_meshRenderer->AssignMaterial("defaultShader");
	debugSphere->m_meshRenderer->AssignTexture("red", "texture");
	debugSphere->m_meshRenderer->AssignTexture("earthNormals", "normals");
	debugSphere->m_meshRenderer->m_renderType = 0x003;
	debugSphere->m_transform->m_scale = vec3(0.1);
	renderingManager->DebugSetupSphere(*debugSphere);

	renderingManager->RequestRenderTicket(*floor_obj);
	mainScene->AddObject(floor_obj);
	floor_obj->m_transform->m_position = (vec3(0, -15, 0));
	floor_obj->m_rigidBody->m_isPinned = true;

	GameChar* lightObj = new GameChar(&cube);
	lightObj->m_meshRenderer->AssignMaterial("defaultShader");
	lightObj->m_meshRenderer->AssignTexture("blankDiffuse", "texture");
	lightObj->m_meshRenderer->AssignTexture("blankDiffuse", "normals");
	lightObj->m_transform->m_position = vec3(-10,5,0);
	lightObj->m_transform->SetRotationUsingEuler(vec3(-1, 0, 0));

	Camera* cameraLight = new Camera();
	cameraLight->m_transform = lightObj->m_transform;
	//cameraLight->m_transform = mainCamera->m_transform;
	lightObj->m_transform->m_position = mainCamera->m_transform->m_position;
	lightObj->m_transform->m_rotation = mainCamera->m_transform->m_rotation;

	mainRenderer->shadowCamera.AttachCamera(cameraLight);
	mainRenderer->shadowCamera.SetOrthography();



	for (int i = 0; i < 11; i++)
	{
		GameChar* object = new GameChar(&sphere);
		object->m_meshRenderer->AssignMaterial("defaultShader");
		object->m_meshRenderer->AssignTexture("earthDiffuse", "texture");
		object->m_meshRenderer->AssignTexture("earthNormals", "normals");
		object->m_transform->m_position = vec3( 3*glm::cos(0.1*i), 8*i, 0);
		mainScene->AddObject(object);
		renderingManager->RequestRenderTicket(*object);
	}

	DirectionalLight* light = new DirectionalLight(vec3(0,-1,0));
	mainScene->AddDirectionalLight(light);

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

		//if (C != mainScene->m_rigidBodySystem->m_collisionProcessor->m_currentContacts.size())
		//{
		//	C = mainScene->m_rigidBodySystem->m_collisionProcessor->m_currentContacts.size();
		//	cout << "Contacts: " <<C<< "\n";
		//}

		//if (mainScene->m_rigidBodySystem->m_collisionProcessor->m_currentContacts.size() != 0 && (time - lastPressS) > 0.1)
		//{
		//	mainScene->m_enableSimulation = false;
		//	stoped = true;
		//}

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
		}

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

		//auto contacts = mainScene->GetContacts();
		//for (auto contact : *contacts)
		//{
		//	debugRenderingManager->DebugDrawRedSphere(contact.m_contactPositionW);

		//	debug->DrawRay(contact.m_contactPositionW, contact.m_contactNormalW, 1);
		//	debug->DrawRay(contact.m_contactPositionW, contact.m_contactTangent1W, 1);
		//	debug->DrawRay(contact.m_contactPositionW, contact.m_contactTangent2W, 1);
		//}

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

		//cout << "Contacts Size: " << mainScene->GetContacts()->size() << ".\n";

		if (mainScene->m_rigidBodySystem->m_collisionProcessor->debug_stop)
		{
			cout << "LCP Solver unhappy, Simulation Halted\n";
			mainScene->m_enableSimulation = false;
			mainScene->m_rigidBodySystem->m_collisionProcessor->debug_stop = false;
		}

		//if (currentObject)
		//	debug->DrawRay(currentObject->m_transform->m_position, currentObject->m_transform->LocalDirectionToWorld(currentObject->m_rigidBody->m_angularVelocity),10);
		//		
		//if (currentObject != NULL)
		//{
		//	debug->DrawBasis(currentObject->m_transform, 1);
		//}
		//debug->DrawBasis(lightObj->m_transform, 1);
//		debug->DrawRay(lightObj->m_transform->m_position,lightObj->m_transform->LocalDirectionToWorld(vec3(1,0,0)),1);

		SimpleControls(mainCamera);
		
		mainCamera->Update();
		mainRenderer->Update();
		mainScene->Update();

		if(mainScene->m_enableSimulation)
			solvIt += (mainScene->m_rigidBodySystem->m_collisionProcessor->m_iterationCount);

		//cout << "Average Iteration Count" << solvIt / (float)(mainScene->m_rigidBodySystem->m_collisionProcessor->m_solveCount) << "\n";

		//debug->DrawGrid(10, vec4(1));

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
