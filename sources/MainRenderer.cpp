// Include standard headers
#include "std.h"
#include "Graphics.h"
#include "OBJImport.h"
#include "Texture.h"
#include "Shader.h"

bool initializeContext(char* windowTitle );
bool display();
void idle(int* fps_frames,GLfloat* fps_time,int* fps);

GLuint VertexArrayID;
GLuint MatrixID;
GLuint programID;
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint textureID;

vec2 tangentAcceleration;
vec2 mousePosition;

MeshRenderer* mainMesh = new MeshRenderer();


bool initializeContext(char* windowTitle )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetWindowTitle( windowTitle );

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	glfwSetMousePos(1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	return true;
}

bool display()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

	mat4 ViewMatrix =
		glm::lookAt
		(	
			vec3(10,5,0), // Camera is at (10,0,0), in World Space
			vec3(0,0,0), // and looks at the origin
			vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

	ViewMatrix = glm::rotate(ViewMatrix,tangentAcceleration.x,vec3(0,1,0));

	mat4 ModelMatrix = mat4(1.0);

	mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                   // attribute
		2,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		0,                   // stride
		(void*)0             // array buffer offset
		);

	// Draw the triangle !
	glDrawArrays(GL_LINES, 0, mainMesh->meshVertices.size() );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Swap buffers
	glfwSwapBuffers();

	return true;
}

void UpdateAcceleration()
{
	int x,y;

	glfwGetMousePos(&x,&y);
	if(mousePosition.x > x)
	{
		tangentAcceleration.x -= 0.1f;
	}
	else if(mousePosition.x < x)
	{
		tangentAcceleration.x += 0.1f;
	}
	mousePosition.x = x;
}

int main( void )
{
	bool terminationRequest = false;

	initializeContext("Obj renderer");
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	//GLuint Texture = loadDDS("uvmap.DDS");
	GLuint Texture = 0;

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	OBJImport objImporter;
	bool importMesh = objImporter.ImportMesh("bla.obj",mainMesh);

	// Load it into a VBO
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, mainMesh->meshVertices.size() * sizeof(vec3), &(mainMesh->meshVertices[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, mainMesh->meshUVs.size() * sizeof(vec2), &(mainMesh->meshUVs[0]), GL_STATIC_DRAW);

	int fps_frames=0;
	GLfloat fps_time = glfwGetTime();
	int fps = 0;

	while(!terminationRequest)
	{
		idle(&fps_frames,&fps_time,&fps);
		UpdateAcceleration();
		display();

		if( (glfwGetKey( GLFW_KEY_ESC ) == GLFW_PRESS) |  !glfwGetWindowParam( GLFW_OPENED ) )
		{
			terminationRequest = true;
		}
	}

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();

	return 0;
}

void idle(int* fps_frames,GLfloat* fps_time,int* fps)
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