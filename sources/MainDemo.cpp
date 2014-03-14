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
#include "./Engine/WINGL33_Renderer.h"
#define RENDERER 1
#endif

#include "./Game/Scene.h"
#include "./Game/CursorPicker.h"
#include "./Game/GameSingleton.h"
#include "./Game/GameComponents/Collider.h"

int fps = 60;
vec2* previousMouseInput = new vec2(0,0);

GameSingleton* gameSingleton;
Renderer* mainRenderer;
SharedResources* sharedResources;
Debug* debug;
Scene* mainScene;

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

	object->m_rigidBody->PushForce(cameraForce,RigidBody::Force::Impulse);
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

		object->m_rigidBody->PushTorque(cameraTorque,RigidBody::Force::Impulse);
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
	sharedResources             = new SharedResources();
	debug						= new Debug();
    
    
    int mainRenderFullScreen    = FULLSCREEN_SETTING;


	//mainRenderer				= new OSXGL32Renderer((char*)"BLAengine - OBJViewer",mainRenderFullScreen);
	mainRenderer				= new GL33Renderer((char*)"BLAengine - MainDemo",mainRenderFullScreen);

    gameSingleton               = new GameSingleton(mainRenderer,sharedResources);
    
    mainScene					= new Scene();
    
    CursorPicker cursorPicker(gameSingleton);

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	if(!mainRenderer->GetStatus())
	{
		debug->OutputToDebug((char*)"Failed to initiate Context!");
		return -1;
	}

	// NOW WE CAN LOAD SOME RESSOURCES
	sharedResources->LoadMaterial("defaultShader","./resources/shaders/Vertex_Shader.glsl", "./resources/shaders/Fragment_Shader.glsl");
	sharedResources->LoadMaterial("debugShader","./resources/shaders/Debug_Vertex.glsl", "./resources/shaders/Debug_Fragment.glsl");
	
	sharedResources->loadBMP_custom("testDiffuse","./resources/textures/texture.bmp");
	sharedResources->loadBMP_custom("earthDiffuse","./resources/textures/earth.bmp");
	sharedResources->loadBMP_custom("earthNormals","./resources/textures/earth_NRM.bmp");

	mainRenderer->debug = debug;

	GameChar* object_1 = new GameChar();

	OBJImport objImport;

	objImport.ImportMesh("./resources/models/earth.obj",object_1->m_meshRenderer);
	object_1->m_meshRenderer->AssignMaterial("defaultShader");
	object_1->m_meshRenderer->LoadTextureSample("earthDiffuse","texture");
	object_1->m_meshRenderer->LoadTextureSample("earthNormals","normals");
	object_1->m_meshRenderer->GenerateArrays();
	mainRenderer->renderVector.push_back(object_1->m_meshRenderer);

	object_1->m_rigidBody->SetPosition(vec3(0,0,0));
	object_1->m_transform->scale = vec3(1.f);

	object_1->m_rigidBody->frictionCoefficient = 0.01f;

	Collider collider(object_1->m_transform);
	collider.GenerateBoundingBoxFromMesh(object_1->m_meshRenderer);
	collider.UpdateRenderer();
	mainRenderer->renderVector.push_back(collider.m_colliderRenderer);

	mainScene->AddObject(object_1);

	Camera* mainCamera = new Camera();
	mainCamera->m_rigidBody->SetPosition(vec3(0,-10,-15));
	mainCamera->m_rigidBody->SetRotation(vec3(3.14/9,0,0));
	mainCamera->isControlEnabled = true;

	DirectionalLight* light = new DirectionalLight(vec3(1,0,0));
	mainScene->AddDirectionalLight(light);

	mainRenderer->mainCamera = mainCamera;

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();
    
    Ray ray(vec3(0),vec3(0),0);

	while(!terminationRequest)
	{
		Idle(&fps_frames,&fps_time,&fps);

		stringstream title;
		title << "BLAengine: " << fps << " fps";
		title.str();
		glfwSetWindowTitle(mainRenderer->GetWindow(),title.str().data());

		//glfwSetWindowSizeCallback(OnResize);

		mainScene->Update();

		if( (glfwGetKey(mainRenderer->GetWindow(), 'F'  ) == GLFW_PRESS) )
		{
			object_1->m_rigidBody->PushTorque(vec3(0,10,0),RigidBody::Force::Impulse);
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
            
        if(glfwGetMouseButton(gameSingleton->renderer->GetWindow(), 0))
        {
            ray = cursorPicker.ScreenToRay(1000);
			vec3 hitOnPlane = ray.RayToPlaneIntersection(vec3(0),vec3(1,0,0),vec3(0,0,1)).hitPosition;
			object_1->m_rigidBody->SetPosition(hitOnPlane);
		}

		debug->DrawRay(light->GetTransform()->position,light->GetDirection(),10);
        debug->DrawRay(ray.origin,ray.direction,ray.length);

		SimpleControls(mainCamera);
        
		mainCamera->Update();
		mainRenderer->Update();		

		debug->DrawGrid(10,vec4(0.9,0.9,0.9,0.05f));

		debug->DrawBasis(object_1->m_transform,1.f);

		if( (glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_ESCAPE ) == GLFW_PRESS)  |  glfwWindowShouldClose(mainRenderer->GetWindow()) )
		{
			terminationRequest = true;
		}
	}


	glfwTerminate();

	return 0;
}
