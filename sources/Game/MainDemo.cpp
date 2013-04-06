// Include standard headers
#include "std.h"
#include "Graphics.h"
#include "Renderer.h"
#include "GameChar.h"
#include "SharedRessources.h"

vec2 tangentAcceleration;
vec2 mousePosition;
int previousX = 0, previousY = 0;
int fps = 60;

Debug* debug;
Renderer* render;
SharedRessources* sharedRessources;

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

void SimpleControls(Camera* camera)
{
	vec3 tangentForce = vec3(0);
	int coeff = 5;

	if ( (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS) )
		coeff = 25;

	if( (glfwGetKey( 'W'  ) == GLFW_PRESS) )
		tangentForce.z = 1;
	if( (glfwGetKey( 'S'  ) == GLFW_PRESS) )
		tangentForce.z = -1;

	if( (glfwGetKey( 'A'  ) == GLFW_PRESS) )
		tangentForce.x = 1;
	if( (glfwGetKey( 'D'  ) == GLFW_PRESS) )
		tangentForce.x = -1;

	vec3 cameraForce = camera->transform->LocalDirectionToWorld(tangentForce);
	cameraForce *= coeff;

	camera->rigidBody->PushForce(cameraForce,RigidBody::Force::Impulse);


	if( (glfwGetMouseButton( 1 ) == GLFW_PRESS) )
	{
		int x, y;
		glfwGetMousePos(&x,&y);

		vec3 cameraTorque = vec3(0);

		if(x - previousX > 0)
		{
			cameraTorque.y = 1.f;
		}
		else if(x - previousX < 0)
		{
			cameraTorque.y = -1.f;
		}
		if(y - previousY > 0)
		{
			cameraTorque.x = 1.f;
		}
		else if(y - previousY < 0)
		{
			cameraTorque.x = -1.f;
		}
		previousX = x;
		previousY = y;

		cameraTorque *= 5;

		camera->rigidBody->PushTorque(cameraTorque,RigidBody::Force::Impulse);

	}
	else
	{

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

void Idle(int* fps_frames,GLfloat* fps_time,int* fps)
{
	GetFPS(fps_frames,fps_time, fps);
	//LockFramerate(100.f); // Lock framerate 100 hz
}


// ACHTUNG ! Executed each frame, WTF ? investigate.
// Oh BTW, it's broken
void GLFWCALL OnResize(int xRes,int yRes)
{
	render->Resize(xRes,yRes);
}

int main( void )
{
	render				= new Renderer();
	sharedRessources	= new SharedRessources();
	debug				= new Debug();

	bool terminationRequest = false;

	// OPENGL CONTEXT UP AND RUNNING
	render->InitializeContext("BLAengine - OBJViewer");

	// NOW WE CAN LOAD SOME RESSOURCES
	sharedRessources->LoadMaterial("defaultShader","../resources/shaders/Vertex_Shader.glsl", "../resources/shaders/Fragment_Shader.glsl");
	sharedRessources->LoadMaterial("debugShader","../resources/shaders/Debug_Vertex.glsl", "../resources/shaders/Debug_Fragment.glsl");
	sharedRessources->LoadMaterial("NoProjShader","../resources/shaders/NoProj_Vertex.glsl", "../resources/shaders/NoProj_Fragment.glsl");


	render->debug = debug;

	GameChar* object_1 = new GameChar();
	GameChar* object_2 = new GameChar();

	render->screenSize.x = 1000;
	render->screenSize.y = 1000;

	OBJImport::ImportMesh("../resources/models/dude.obj",object_1->meshRenderer);
	object_1->meshRenderer->AssignMaterial("defaultShader");
	object_1->meshRenderer->GenerateArrays();
	render->renderVector.push_back(object_1->meshRenderer);


	OBJImport::ImportMesh("../resources/models/bla.obj",object_2->meshRenderer);
	object_2->meshRenderer->AssignMaterial("defaultShader");
	object_2->meshRenderer->GenerateArrays();
	render->renderVector.push_back(object_2->meshRenderer);

	object_1->transform->scale	= vec3(0.01);
	object_2->transform->scale	= vec3(0.4);

	object_1->rigidBody->SetPosition(vec3(10,0,0));
	object_2->rigidBody->SetPosition(vec3(-2,0,0));

	object_1->rigidBody->frictionCoefficient = 0.01f;

	Camera* mainCamera = new Camera();
	mainCamera->rigidBody->SetPosition(vec3(0,-10,-15));
	mainCamera->rigidBody->SetRotation(vec3(3.14/9,0,0));
	
	render->mainCamera = mainCamera;

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();

	while(!terminationRequest)
	{
		Idle(&fps_frames,&fps_time,&fps);

		stringstream title;
		title << "BLAengine: " << fps << " fps";
		title.str();
		glfwSetWindowTitle(title.str().data());

		glfwSetWindowSizeCallback(OnResize);

		object_1->Update();
		object_2->Update();

		if( (glfwGetKey( 'F'  ) == GLFW_PRESS) )
			object_1->rigidBody->PushTorque(vec3(0,10,0),RigidBody::Force::Impulse);

		SimpleControls(mainCamera);

		mainCamera->Update();
		render->Update();

	//	debug->DrawLine(object_2->transform->position,object_1->transform->LocalPositionToWorld(vec3(50.f,10.f,1.f)),vec3(1.f,1.f,1.f));
			
		debug->DrawGrid(10,vec4(0.9,0.9,0.9,0.3f));
		debug->DrawBasis(object_1->transform,1.f);

		if( (glfwGetKey( GLFW_KEY_ESC ) == GLFW_PRESS) | !glfwGetWindowParam( GLFW_OPENED ) )
		{
			terminationRequest = true;
		}
	}

	glfwTerminate();

	return 0;
}
