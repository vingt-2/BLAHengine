#include "Renderer.h"


Renderer::Renderer(void)
{
	screenSize = vec2(1000,1000);
}


Renderer::~Renderer(void)
{
}

bool Renderer::InitializeContext(char* windowTitle )
{

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 16 );
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context

	if( !glfwOpenWindow(screenSize.x, screenSize.y, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
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
	glfwSetMousePos(screenSize.x/2, screenSize.y/2);

	//Neat grey background
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Enable z-buf test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	return true;
}

bool Renderer::Update()
{
	// Clear the screen

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mainCamera->SetProjection(glm::frustum(-0.0001f*screenSize.x, 0.0001f*screenSize.x,-0.0001f*screenSize.y, 0.0001f*screenSize.y, 0.1f, 100.0f));
	glViewport(0,0,screenSize.x,screenSize.y);

	for(int i = 0; i < renderVector.size();i++)
	{
		renderVector[i]->Draw(mainCamera->projection,mainCamera->viewTransform.transformMatrix);
	}

	glfwSwapBuffers();

	return true;
}

void Renderer::Resize(int xRes,int yRes)
{
	 glfwSetWindowSize(xRes,yRes);
	 screenSize.x = xRes;
	 screenSize.y = yRes;
}
