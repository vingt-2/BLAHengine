// Include standard headers
#include "std.h"
#include "Graphics.h"
#include "Renderer.h"

vec2 tangentAcceleration;
vec2 mousePosition;

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
	LockFramerate(100.f); // Lock framerate 100 hz
}

int main( void )
{
	bool terminationRequest = false;

	Renderer render;
	render.InitializeContext("BLAengine - OBJViewer");

	OBJImport objImporter;
	MeshRenderer* mesh_1 = new MeshRenderer();
	MeshRenderer* mesh_2 = new MeshRenderer();


	objImporter.ImportMesh("../models/bla.obj",mesh_1);
	mesh_1->LoadShaders( "../shaders/Vertex_Shader.glsl", "../shaders/Fragment_Shader.glsl" );
	mesh_1->GenerateArrays();
	render.renderVector.push_back(mesh_1);
	objImporter.ImportMesh("cube.obj",mesh_2);
	mesh_2->LoadShaders( "../shaders/Vertex_Shader.glsl", "../shaders/Fragment_Shader.glsl" );
	mesh_2->GenerateArrays();
	render.renderVector.push_back(mesh_2);

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