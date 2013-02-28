// Include standard headers
#include "std.h"
#include "Graphics.h"
#include "Renderer.h"

vec2 tangentAcceleration;
vec2 mousePosition;
Renderer render;

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

void UpdateAcceleration()
{
	tangentAcceleration.x = 0.1f;
	tangentAcceleration.y = 0.05f;
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

void GLFWCALL OnResize(int xRes,int yRes)
{
	render.Resize(xRes,yRes);
	cout << "IM AM RESSIIIZZZING \n";
}

int main( void )
{
	bool terminationRequest = false;

	Renderer render;
	render.InitializeContext("BLAengine - OBJViewer");
	GameObject* object_1 = new GameObject();


	OBJImport::ImportMesh("../models/cube.obj",object_1->meshRenderer);
	object_1->meshRenderer->LoadShaders( "../shaders/Vertex_Shader.glsl", "../shaders/Fragment_Shader.glsl" );
	object_1->meshRenderer->GenerateArrays();
	render.renderVector.push_back(object_1->meshRenderer);



	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();
	int fps = 0;

	vec3 objectPosition = vec3(0);

	while(!terminationRequest)
	{
		Idle(&fps_frames,&fps_time,&fps);

		stringstream title;
		title << "BLAengine: " << fps << " fps";
		title.str();
		glfwSetWindowTitle(title.str().data());

		glfwSetWindowSizeCallback(OnResize);

		objectPosition.x = objectPosition.x + 0.01f;

		object_1->SetPosition(objectPosition);


		UpdateAcceleration();

		render.Update();

		render.viewMatrix = glm::rotate(render.viewMatrix,tangentAcceleration.x,vec3(0,1,0));
		render.viewMatrix = glm::rotate(render.viewMatrix,tangentAcceleration.y,vec3(0,0,1));


		if( (glfwGetKey( GLFW_KEY_ESC ) == GLFW_PRESS) | !glfwGetWindowParam( GLFW_OPENED ) )
		{
			terminationRequest = true;
		}
	}

	glfwTerminate();

	return 0;
}
