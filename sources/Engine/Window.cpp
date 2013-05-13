#include "../Headers/Window.h"

Window::Window()
{
	renderer = new Renderer();
}

GLFWwindow* Window::InitializeContext(char* windowTitle,vec2 screenSize)
{

	GLFWwindow* window;
	// Initialise GLFW
	if( !glfwInit() )
	{
		Debug::OutputToDebug("Failed to initialize GLFW\n" );
		return false;
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
		return false;
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