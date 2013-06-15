/*
This is a terribly awful quick testing pot-pourris of the features I put into the Engine,
It's not part of the final architecture but is merely there to test the new features as they come.
Hence the name "MainDemo.cpp".
*/


// Include standard headers
#pragma once
#include "./Std/std.h"
#include "./Engine/Renderer.h"
#include "./Engine/SharedRessources.h"
#include "./Game/Scene.h"

int fps = 60;
vec2* previousMouseInput = new vec2(0,0);

Renderer* mainRenderer;
SharedRessources* sharedRessources;
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

void SimpleControls(Camera* object)
{

	vec3 tangentForce = vec3(0);
	int coeff = 5;

	if ( (glfwGetKey(mainRenderer->GetWindow(),GLFW_KEY_LSHIFT) == GLFW_PRESS) )
		coeff = 25;

	if( (glfwGetKey(mainRenderer->GetWindow(), 'W'  ) == GLFW_PRESS) )
		tangentForce.z = 1;
	if( (glfwGetKey(mainRenderer->GetWindow(), 'S'  ) == GLFW_PRESS) )
		tangentForce.z = -1;

	if( (glfwGetKey(mainRenderer->GetWindow(), 'A'  ) == GLFW_PRESS) )
		tangentForce.x = 1;
	if( (glfwGetKey(mainRenderer->GetWindow(), 'D'  ) == GLFW_PRESS) )
		tangentForce.x = -1;

	vec3 cameraForce = object->transform->LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;

	object->rigidBody->PushForce(cameraForce,RigidBody::Force::Impulse);


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

		object->rigidBody->PushTorque(cameraTorque,RigidBody::Force::Impulse);
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

// ACHTUNG ! Executed each frame, WTF ? investigate.
// Oh BTW, it's broken
//void GLFWCALL OnResize(int xRes,int yRes)
//{
//	mainRenderer->Resize(xRes,yRes);
//}

int main( void )
{
	sharedRessources			= new SharedRessources();
	debug						= new Debug();
	mainRenderer				= new Renderer("BLAengine - OBJViewer");
	mainScene					= new Scene();

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	if(!mainRenderer->GetStatus())
	{
		debug->OutputToDebug("Failed to initiate Context!");
		return -1;
	}

	// NOW WE CAN LOAD SOME RESSOURCES
	sharedRessources->LoadMaterial("defaultShader","./resources/shaders/Vertex_Shader.glsl", "./resources/shaders/Fragment_Shader.glsl");
	sharedRessources->LoadMaterial("debugShader","./resources/shaders/Debug_Vertex.glsl", "./resources/shaders/Debug_Fragment.glsl");

	mainRenderer->debug = debug;

	GameChar* object_1 = new GameChar();
	GameChar* object_2 = new GameChar();

	OBJImport::ImportMesh("./resources/models/dude.obj",object_1->meshRenderer);
	object_1->meshRenderer->AssignMaterial("defaultShader");
	object_1->meshRenderer->GenerateArrays();
	mainRenderer->renderVector.push_back(object_1->meshRenderer);


	OBJImport::ImportMesh("./resources/models/bla.obj",object_2->meshRenderer);
	object_2->meshRenderer->AssignMaterial("defaultShader");
	object_2->meshRenderer->GenerateArrays();
	mainRenderer->renderVector.push_back(object_2->meshRenderer);

	object_1->transform->scale	= vec3(0.01);
	object_2->transform->scale	= vec3(0.4);

	object_1->rigidBody->SetPosition(vec3(10,0,0));
	object_2->rigidBody->SetPosition(vec3(-2,0,0));

	object_1->rigidBody->frictionCoefficient = 0.01f;

	mainScene->AddObject(object_1);
	mainScene->AddObject(object_2);

	Camera* mainCamera = new Camera();
	mainCamera->rigidBody->SetPosition(vec3(0,-10,-15));
	mainCamera->rigidBody->SetRotation(vec3(3.14/9,0,0));
	mainCamera->isControlEnabled = true;

	mainRenderer->mainCamera = mainCamera;

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();

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
			object_1->rigidBody->PushTorque(vec3(0,10,0),RigidBody::Force::Impulse);
		}

		SimpleControls(mainCamera);


		mainCamera->Update();
		mainRenderer->Update();

		debug->DrawLine(object_2->transform->position,object_1->transform->LocalPositionToWorld(vec3(50.f,10.f,1.f)),vec4(1.f,1.f,1.f,1.f));
			

		debug->DrawGrid(10,vec4(0.9,0.9,0.9,0.3f));
		debug->DrawBasis(object_1->transform,1.f);

		if( (glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_ESC ) == GLFW_PRESS)  |  glfwWindowShouldClose(mainRenderer->GetWindow()) )
		{
			terminationRequest = true;
		}
	}

	glfwTerminate();

	return 0;
}
