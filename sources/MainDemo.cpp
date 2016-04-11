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
GameChar* currentObject = NULL;

vec3 cameraRotation = vec3(0);

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
	float coeff = 0.5f;


	if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		coeff = 1.f;

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

	vec3 cameraForce = object->m_transform->LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;

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

		deltaRotation *= 0.05;

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
	
	mainScene = new Scene(mainCamera);

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
		return -1;
	}

	// NOW WE CAN LOAD SOME RESSOURCES
	sharedResources->LoadMaterial("defaultShader","../resources/shaders/Vertex_Shader.glsl", "../resources/shaders/Fragment_Shader.glsl");
	sharedResources->LoadMaterial("debugShader","../resources/shaders/Debug_Vertex.glsl", "../resources/shaders/Debug_Fragment.glsl");
	
	sharedResources->loadBMP_custom("testDiffuse","../resources/textures/damier.bmp");
	sharedResources->loadBMP_custom("blankDiffuse", "../resources/textures/blank.bmp");
	sharedResources->loadBMP_custom("earthDiffuse","../resources/textures/earth.bmp");
	sharedResources->loadBMP_custom("earthNormals","../resources/textures/earth_NRM.bmp");
	sharedResources->loadBMP_custom("red", "../resources/textures/red.bmp");

	//mainRenderer->m_debug = debug;
	OBJImport objImport;

	MeshAsset xwing;
	MeshAsset sphere;
	MeshAsset floor;
	MeshAsset cube;
	//objImport.ImportMesh("../resources/models/x-wing.obj", &xwing);
	objImport.ImportMesh("../resources/models/cube.obj", &cube);
	objImport.ImportMesh("../resources/models/cube.obj", &floor);
	objImport.ImportMesh("../resources/models/bla.obj", &sphere);
	mat4 scaleMat(vec4(10, 0, 0, 0), 
					vec4(0, 0.2, 0, 0), 
					vec4(0, 0, 10, 0),
					vec4(0, 0, 0, 1));
	for (auto &v : floor.m_meshVertices){
		vec4 hP = scaleMat * vec4(v,1);
		v = vec3(hP.x, hP.y, hP.z);
	}
	for (auto &v : floor.m_meshUVs){
		v = mat2(100) * v;
	}

	GameChar* object_1 = new GameChar(&floor);
	GameChar* object_2 = new GameChar(&cube);

	object_1->m_meshRenderer->AssignMaterial("defaultShader");
	
	object_1->m_meshRenderer->AssignTexture("testDiffuse","texture");
	object_1->m_meshRenderer->AssignTexture("earthNormals","normals");
	
	object_2->m_meshRenderer->AssignMaterial("defaultShader");
	object_2->m_meshRenderer->AssignTexture("blankDiffuse", "texture");
	object_2->m_meshRenderer->AssignTexture("earthNormals", "normals");

	GameChar* debugSphere = new GameChar(&cube);
	debugSphere->m_meshRenderer->AssignMaterial("defaultShader");
	debugSphere->m_meshRenderer->AssignTexture("red", "texture");
	debugSphere->m_meshRenderer->AssignTexture("earthNormals", "normals");
	debugSphere->m_meshRenderer->m_renderType = 0x003;
	debugSphere->m_transform->m_scale = vec3(0.1);
	renderingManager->DebugSetupSphere(*debugSphere);

	renderingManager->RequestRenderTicket(*object_1);
	renderingManager->RequestRenderTicket(*object_2);
	mainScene->AddObject(object_1);
	mainScene->AddObject(object_2);

	object_1->m_transform->m_position = (vec3(0, -5, 0));
	object_2->m_transform->m_position = (vec3(0, 0, 0));

	DirectionalLight* light = new DirectionalLight(vec3(1,0,0));
	mainScene->AddDirectionalLight(light);

	mainRenderer->m_mainCamera = mainCamera;

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();
	
	Ray ray(vec3(0),vec3(0),0);

	//
	// Did you know ?
	// There is a big ducking memory leak when you draw debug rays.
	// So don't do it too much.
	// Cheers
	// 

	int frameCount = 0;
	vector<Ray> debugRays;
	int lastPress = 0;

	while(!terminationRequest)
	{
		object_1->m_rigidBody->m_isPinned = true;
		Idle(&fps_frames,&fps_time,&fps);

		stringstream title;
		title << "BLAengine: " << fps << " fps";
		title.str();
		glfwSetWindowTitle(mainRenderer->GetWindow(),title.str().data());

		//glfwSetWindowSizeCallback(OnResize);

//		object_1->m_rigidBody->AddTorque(vec3(0, 2, 0));
//		object_2->m_rigidBody->AddTorque(vec3(0, -2, 0));

		mainScene->Update();

		if(currentObject == NULL)
			ObjectControl(object_2);
		else
			ObjectControl(currentObject);

		if ((glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) && (frameCount-lastPress) > 60)
		{
			if (mainScene->m_enableSimulation)
			{
				mainScene->m_enableSimulation = false;
			}
			else
			{
				mainScene->m_enableSimulation = true;
			}
			lastPress = frameCount;
		}

		if( (glfwGetKey(mainRenderer->GetWindow(), 'J'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->m_rotation += vec3(0.f,0.1f,0.f);
		}
		else if( (glfwGetKey(mainRenderer->GetWindow(), 'L'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->m_rotation += vec3(0.f,-0.1f,0.f);
		}
		if( (glfwGetKey(mainRenderer->GetWindow(), 'I'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->m_rotation += vec3(0.f,0.f,0.1f);
		}
		else if( (glfwGetKey(mainRenderer->GetWindow(), 'K'  ) == GLFW_PRESS) )
		{
			light->GetTransform()->m_rotation += vec3(0.f,0.f,-0.1f);
		}

		//cout << collider_1->CollidesWith(collider_2);
		
		if(glfwGetMouseButton(gameSingleton->renderer->GetWindow(), 0))
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
				object->m_rigidBody->PushForceWorld(colPoint, 10.f*ray.m_direction);
			}
		}

		if (glfwGetMouseButton(gameSingleton->renderer->GetWindow(), 2))
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

		for (auto r : debugRays)
		{
			debug->DrawRay(r.m_origin, r.m_direction, r.m_length);
		}
		//cout << "Contacts Size: " << mainScene->GetContacts()->size() << ".\n";
		for (int c = 0; c < mainScene->GetContacts()->size(); c+=2)
		{
			Contact contact = mainScene->GetContacts()->at(c);

			if (contact.m_contactPositionW != vec3(0, 0, 0))
			{
				renderingManager->DebugDrawRedSphere(contact.m_contactPositionW);
				//debug->DrawRay(contact.m_contactPositionW, contact.m_contactNormalBody2W, 1);
				//debug->DrawRay(contact.m_contactPositionW, contact.m_contactTangent1Body2W, 1);
				//debug->DrawRay(contact.m_contactPositionW, contact.m_contactTangent2Body2W, 1);
			}

			vec3 vrel = object_2->m_rigidBody->m_velocity - object_1->m_rigidBody->m_velocity;
			float vrelN = dot(vrel,contact.m_contactNormalBody2W)/mainScene->GetContacts()->size();
		}
		
		debug->DrawRay(object_2->m_transform->m_position, object_2->m_rigidBody->m_debugCorrectionVelocity, 10);
		cout << object_2->m_rigidBody->m_debugCorrectionVelocity.x << ", " << object_2->m_rigidBody->m_debugCorrectionVelocity.y << ", " << object_2->m_rigidBody->m_debugCorrectionVelocity.z << "\n";
		if (currentObject != NULL)
		{
			debug->DrawBasis(currentObject->m_transform, 1);
		}

		SimpleControls(mainCamera);
		
		mainCamera->Update();
		mainRenderer->Update();

		//debug->DrawGrid(10, vec4(1));

		frameCount++;
		if( (glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_ESCAPE ) == GLFW_PRESS)  |  glfwWindowShouldClose(mainRenderer->GetWindow()) )
		{
			terminationRequest = true;
		}
	}


	glfwTerminate();

	return 0;
}
