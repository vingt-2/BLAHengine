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
Renderer* mainRenderer;
SharedResources* sharedResources;
Debug* debug;
Scene* mainScene;
RigidBodySystem* rigidBodySystem;

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

void SimpleControls(GameChar* object)
{

	vec3 tangentForce = vec3(0);
	vec3 movementForce = vec3(0);
	int coeff = 5;


	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		coeff = 25;

	if( (glfwGetKey(mainRenderer->GetWindow(), 'W'  ) == GLFW_PRESS) )
		tangentForce.z = 1;
	if( (glfwGetKey(mainRenderer->GetWindow(), 'S'  ) == GLFW_PRESS) )
		tangentForce.z = -1;

	if( (glfwGetKey(mainRenderer->GetWindow(), 'A'  ) == GLFW_PRESS) )
		tangentForce.x = 1;
	if( (glfwGetKey(mainRenderer->GetWindow(), 'D'  ) == GLFW_PRESS) )
		tangentForce.x = -1;

	vec3 cameraForce = object->m_transform->LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;

	object->m_rigidBody->AddForce(cameraForce);
	//object->


	if( (glfwGetMouseButton(mainRenderer->GetWindow(), 1 ) == GLFW_PRESS) )
	{
		double x, y;
		glfwGetCursorPos(mainRenderer->GetWindow(),&x,&y);

		vec3 cameraTorque = vec3(0);

		if(x - previousMouseInput->x > 0)
		{
			cameraTorque.y = 1.f;
		}
		else if(x - previousMouseInput->x < 0)
		{
			cameraTorque.y = -1.f;
		}
		if(y - previousMouseInput->y > 0)
		{
			cameraTorque.x = 1.f;
		}
		else if(y - previousMouseInput->y < 0)
		{
			cameraTorque.x = -1.f;
		}
		previousMouseInput->x = x;
		previousMouseInput->y = y;

		cameraTorque *= 5;

		object->m_rigidBody->AddTorque(cameraTorque);
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


int main( void )
{
	sharedResources = new SharedResources();
	
	int mainRenderFullScreen    = FULLSCREEN_SETTING;

	mainRenderer				= new GL33Renderer((char*)"BLAengine - MainDemo",mainRenderFullScreen);

	gameSingleton               = new GameSingleton(mainRenderer,sharedResources);
	
	mainScene					= new Scene();

	RenderingManager* renderingManager = new RenderingManager(1, mainRenderer, RenderingManager::Game);
	RenderingManager* debugRenderingManager = new RenderingManager(1, mainRenderer, RenderingManager::DebugGizmo);

	debug = new Debug(debugRenderingManager);
	rigidBodySystem = new RigidBodySystem();
	
	CursorPicker cursorPicker(gameSingleton);

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	if(!mainRenderer->GetStatus())
	{
		printf("Failed to initiate Context!");
		//debug->OutputToDebug((char*)"Failed to initiate Context!");
		return -1;
	}

	// NOW WE CAN LOAD SOME RESSOURCES
	sharedResources->LoadMaterial("defaultShader","../resources/shaders/Vertex_Shader.glsl", "../resources/shaders/Fragment_Shader.glsl");
	sharedResources->LoadMaterial("debugShader","../resources/shaders/Debug_Vertex.glsl", "../resources/shaders/Debug_Fragment.glsl");
	
	sharedResources->loadBMP_custom("testDiffuse","../resources/textures/test.bmp");
	sharedResources->loadBMP_custom("blankDiffuse", "../resources/textures/blank.bmp");
	sharedResources->loadBMP_custom("earthDiffuse","../resources/textures/earth.bmp");
	sharedResources->loadBMP_custom("earthNormals","../resources/textures/earth_NRM.bmp");

	//mainRenderer->m_debug = debug;
	OBJImport objImport;

	MeshAsset mesh1;
	objImport.ImportMesh("../resources/models/cube.obj", &mesh1);
	MeshAsset mesh2;
	objImport.ImportMesh("../resources/models/cube.obj", &mesh2);
	GameChar* object_1 = new GameChar(&mesh1);
	GameChar* object_2 = new GameChar(&mesh2);

	object_1->m_meshRenderer->AssignMaterial("defaultShader");
	object_1->m_meshRenderer->AssignTexture("blankDiffuse","texture");
	object_1->m_meshRenderer->AssignTexture("earthNormals","normals");
	object_2->m_meshRenderer->AssignMaterial("defaultShader");
	object_2->m_meshRenderer->AssignTexture("earthDiffuse", "texture");
	object_2->m_meshRenderer->AssignTexture("earthNormals", "normals");
	object_2->m_transform->scale = vec3(0.1);


	renderingManager->RequestRenderTicket(*object_1);
	renderingManager->RequestRenderTicket(*object_2);

	object_1->m_rigidBody->SetPosition(vec3(0, 0, 0));
	object_2->m_rigidBody->SetPosition(vec3(-3, 0, 0));

	rigidBodySystem->RegisterRigidBody(*(object_1->m_rigidBody));
	rigidBodySystem->RegisterRigidBody(*(object_2->m_rigidBody));

	mainScene->AddObject(object_1);
	mainScene->AddObject(object_2);

	Camera* mainCamera = new Camera();
	mainCamera->m_rigidBody->SetPosition(vec3(0, -10, -15));
	mainCamera->m_rigidBody->SetRotation(vec3(3.14 / 9, 0, 0));
	mainCamera->m_isControlEnabled = true;

	rigidBodySystem->RegisterRigidBody(*(mainCamera->m_rigidBody));

	DirectionalLight* light = new DirectionalLight(vec3(1,0,0));
	mainScene->AddDirectionalLight(light);

	mainRenderer->m_mainCamera = mainCamera;

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();
	
	 Ray ray(vec3(0),vec3(0),0);

	while(!terminationRequest)
	{
		rigidBodySystem->SolveSystem();

		Idle(&fps_frames,&fps_time,&fps);

		stringstream title;
		title << "BLAengine: " << fps << " fps";
		title.str();
		glfwSetWindowTitle(mainRenderer->GetWindow(),title.str().data());

		//glfwSetWindowSizeCallback(OnResize);

//		object_1->m_rigidBody->AddTorque(vec3(0, 2, 0));
//		object_2->m_rigidBody->AddTorque(vec3(0, -2, 0));

		mainScene->Update();

		if( (glfwGetKey(mainRenderer->GetWindow(), 'F'  ) == GLFW_PRESS) )
		{
			object_2->m_rigidBody->AddTorque(vec3(0,10,0));
		}

		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_UP) == GLFW_PRESS))
		{
			object_2->m_rigidBody->AddForce(vec3(0, 5, 0));
		}
		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS))
		{
			object_2->m_rigidBody->AddForce(vec3(-5, 0, 0));
		}
		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS))
		{
			object_2->m_rigidBody->AddForce(vec3(5, 0, 0));
		}
		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS))
		{
			object_2->m_rigidBody->AddForce(vec3(0, -5, 0));
		}

		if( (glfwGetKey(mainRenderer->GetWindow(), 'J'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->rotation += vec3(0.f,0.1f,0.f);
		}
		else if( (glfwGetKey(mainRenderer->GetWindow(), 'L'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->rotation += vec3(0.f,-0.1f,0.f);
		}
		if( (glfwGetKey(mainRenderer->GetWindow(), 'I'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->rotation += vec3(0.f,0.f,0.1f);
		}
		else if( (glfwGetKey(mainRenderer->GetWindow(), 'K'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->rotation += vec3(0.f,0.f,-0.1f);
		}

		//cout << collider_1->CollidesWith(collider_2);
		
		if(glfwGetMouseButton(gameSingleton->renderer->GetWindow(), 0))
		{
			ray = cursorPicker.ScreenToRay(1000);
			//vec3 collide = collider_1->RayCollision(ray);
			vec3 collide(0);
			vec3 hitOnPlane;
			if (collide == vec3(0))
				hitOnPlane = ray.RayToPlaneIntersection(vec3(0), vec3(1, 0, 0), vec3(0, 0, 1)).hitPosition;
			else
				hitOnPlane = collide;
			if (!isnan(hitOnPlane.x) && !isnan(hitOnPlane.y) && !isnan(hitOnPlane.z) ) object_2->m_rigidBody->SetPosition(hitOnPlane);
		}
		
		
		//debug->DrawGrid(10, vec4(0.9, 0.9, 0.9, 0.05f));
		//debug->DrawRay(light->GetTransform()->position,light->GetDirection(),10);
		//debug->DrawRay(ray.m_origin, ray.m_direction, ray.m_length);
		//debug->DrawBasis(object_1->m_transform, 1.f);

		SimpleControls(mainCamera);
		
		mainCamera->Update();
		mainRenderer->Update();

		if( (glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_ESCAPE ) == GLFW_PRESS)  |  glfwWindowShouldClose(mainRenderer->GetWindow()) )
		{
			terminationRequest = true;
		}
	}


	glfwTerminate();

	return 0;
}
