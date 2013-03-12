// Include standard headers
#include "std.h"
#include "Graphics.h"
#include "Renderer.h"

vec2 tangentAcceleration;
vec2 mousePosition;
Renderer render;

int previousX = 0, previousY = 0;

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

void SimpleControls(GameObject* camera)
{
	tangentAcceleration.x = 0.f;
	tangentAcceleration.y = 0.f;
	int coeff = 1;

	if ( (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS))
		coeff = 5;

	if( (glfwGetKey( 'W'  ) == GLFW_PRESS) )
		tangentAcceleration.y = coeff * 0.1f;
	if( (glfwGetKey( 'S'  ) == GLFW_PRESS) )
		tangentAcceleration.y = coeff * -0.1f;

	if( (glfwGetKey( 'A'  ) == GLFW_PRESS) )
		tangentAcceleration.x = coeff * 0.1f;
	if( (glfwGetKey( 'D'  ) == GLFW_PRESS) )
		tangentAcceleration.x = coeff * -0.1f;

	if( (glfwGetMouseButton( 1 ) == GLFW_PRESS) )
	{
		int x, y;
		glfwGetMousePos(&x,&y);

		if(x - previousX > 0)
		{
			camera->transform.rotation.y += 0.5f;
		}
		else if(x - previousX < 0)
		{
			camera->transform.rotation.y -= 0.5f;
		}
		if(y - previousY > 0)
		{
			camera->transform.rotation.x += 0.5f;
		}
		else if(y - previousY < 0)
		{
			camera->transform.rotation.x -= 0.5f;
		}
		previousX = x;
		previousY = y;
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
	render.Resize(xRes,yRes);
}

int main( void )
{
	bool terminationRequest = false;

	Renderer render;
	render.InitializeContext("BLAengine - OBJViewer");

	GameObject* object_1 = new GameObject();
	GameObject* object_2 = new GameObject();

	
	OBJImport::ImportMesh("../models/dude.obj",object_1->meshRenderer);
	object_1->meshRenderer->LoadShaders( "../shaders/Vertex_Shader.glsl", "../shaders/Fragment_Shader.glsl" );
	object_1->meshRenderer->GenerateArrays();
	render.renderVector.push_back(object_1->meshRenderer);
	
	
	OBJImport::ImportMesh("../models/bla.obj",object_2->meshRenderer);
	object_2->meshRenderer->LoadShaders( "../shaders/Vertex_Shader.glsl", "../shaders/Fragment_Shader.glsl" );
	object_2->meshRenderer->GenerateArrays();
	render.renderVector.push_back(object_2->meshRenderer);

	object_1->transform.scale	= vec3(0.01);
	object_2->transform.scale	= vec3(0.4);

	Camera* mainCamera = new Camera();

	mainCamera->SetProjection(glm::perspective(45.f, 4.0f / 3.0f, 0.1f, 100.0f));

	mainCamera->SetView(
		glm::lookAt
		(	
			vec3(0,2,-10), 
			vec3(0,0,0),
			vec3(0,1,0)
		));
	
	render.mainCamera = mainCamera;

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();
	int fps = 0;

	while(!terminationRequest)
	{
		Idle(&fps_frames,&fps_time,&fps);
		
		stringstream title;
		title << "BLAengine: " << fps << " fps";
		title.str();
		glfwSetWindowTitle(title.str().data());
		
		glfwSetWindowSizeCallback(OnResize);

		object_1->transform.position = vec3(2,0,0);
		object_2->transform.position = vec3(-2,0,0);


		object_1->Update();
		object_2->Update();

		SimpleControls(mainCamera);

		mainCamera->transform.position = mainCamera->transform.position + vec3(tangentAcceleration.x,0,tangentAcceleration.y);
		mainCamera->Update();
		render.Update();

		if( (glfwGetKey( GLFW_KEY_ESC ) == GLFW_PRESS) | !glfwGetWindowParam( GLFW_OPENED ) )
		{
			terminationRequest = true;
		}
	}

	glfwTerminate();

	return 0;
}
