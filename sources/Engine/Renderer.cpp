#include "Renderer.h"


Renderer::Renderer(char* windowTitle)
{
	isContextEnabled = false;
	screenSize = vec2(1280,1024);
	glfwWindow = InitializeContext(windowTitle, screenSize);
	if(glfwWindow != NULL)
	{
		isContextEnabled = true;
	}
}

Renderer::Renderer(char* windowTitle, vec2 screenSize)
{
	isContextEnabled = false;
	glfwWindow = InitializeContext(windowTitle, screenSize);
	if(glfwWindow != NULL)
	{
		isContextEnabled = true;
	}
}


Renderer::~Renderer(void)
{
}

GLFWwindow* Renderer::InitializeContext(char* windowTitle,vec2 screenSize)
{

	GLFWwindow* window;
	// Initialise GLFW
	if( !glfwInit() )
	{
		Debug::OutputToDebug("Failed to initialize GLFW\n" );
		return NULL;
	}

	glfwWindowHint(GLFW_SAMPLES , 16 );
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR , 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR , 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// Open a window and create its OpenGL context

	window = glfwCreateWindow(screenSize.x, screenSize.y,windowTitle, NULL,NULL);

 
    glfwMakeContextCurrent(window);
	//{
	//	debug->OutputToDebug("Failed to open GLFW window.\n" );
	//	glfwTerminate();
	//	return false;
	//}

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
	{
		Debug::OutputToDebug("Failed to initialize GLEW\n");
		return NULL;
	}

	glfwSetWindowTitle( window,windowTitle );

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	glfwSetCursorPos(window,screenSize.x/2, screenSize.y/2);

	//Neat grey background
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	return window;
}

bool Renderer::Update()
{
	//Adjust Projection and ViewPort.
	mainCamera->SetProjection(glm::frustum(-0.0001f*screenSize.x, 0.0001f*screenSize.x,-0.0001f*screenSize.y, 0.0001f*screenSize.y, 0.1f, 100.0f));
	glViewport(0,0,screenSize.x,screenSize.y);

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Render Scene Objects.

	// Enable Z-Buffer test.
	glEnable(GL_DEPTH_TEST);
	for(int i = 0; i < renderVector.size();i++)
	{
		renderVector[i]->Draw(mainCamera->projection,mainCamera->viewTransform.transformMatrix);
	}

	// Render Debug gizmos

	// Disable Z-Buffer test for debug gizmos.
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(int i = 0; i < debug->gizmoVector.size();i++)
	{
		debug->gizmoVector[i]->Draw(mainCamera->projection,mainCamera->viewTransform.transformMatrix);
	}
	for(int i = 0; i < debug->gizmoVector.size();i++)
	{
		debug->gizmoVector[i]->~MeshRenderer();
	}
	
	debug->gizmoVector.clear();
	glDisable(GL_BLEND);

	glfwSwapInterval(1);

	glfwSwapBuffers(glfwWindow);
	glfwPollEvents();

	return true;
}

void Renderer::Resize(int xRes,int yRes)
{
	 glfwSetWindowSize(glfwWindow,xRes,yRes);
	 screenSize.x = xRes;
	 screenSize.y = yRes;
}
