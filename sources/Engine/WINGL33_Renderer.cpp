#include "WINGL33_Renderer.h"

GL33Renderer::GL33Renderer(char* windowTitle,bool isFullScreen):
    Renderer(windowTitle, isFullScreen)
{
    glfwWindow = InitializeContext(windowTitle);
	if(glfwWindow != NULL)
	{
		isContextEnabled = true;
	}
}

GL33Renderer::~GL33Renderer()
{
}

bool GL33Renderer::Update()
{
	//Set OpenGL to this context.
    glfwMakeContextCurrent(glfwWindow);
    
	//Adjust Projection and ViewPort.
	mainCamera->SetProjection(glm::frustum(-0.0001f*renderSize.x, 0.0001f*renderSize.x,-0.0001f*renderSize.y, 0.0001f*renderSize.y, 0.1f, 100.0f));
	glViewport(0,0,renderSize.x,renderSize.y);
    
	// Clear Screen Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
	//Render Scene Objects.
    
	// Enable Z-Buffer test.
	glEnable(GL_DEPTH_TEST);
	for(int i = 0; i < renderVector.size();i++)
	{
		renderVector[i]->Draw(mainCamera->m_projection,mainCamera->m_viewTransform.transformMatrix);
	}
    
	// Render Debug gizmos
    
	// Disable Z-Buffer test for debug gizmos.
	glDisable(GL_DEPTH_TEST);
    
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	for(int i = 0; i < debug->gizmoVector.size();i++)
	{
		debug->gizmoVector[i]->Draw(mainCamera->m_projection,mainCamera->m_viewTransform.transformMatrix);
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


GLFWwindow* GL33Renderer::InitializeContext(char* windowTitle)
{
    
	GLFWwindow* window;
	// Initialise GLFW
	if( !glfwInit() )
	{
		Debug::OutputToDebug("Failed to initialize GLFW\n" );
		return NULL;
	}
    
	//glfwWindowHint(GLFW_SAMPLES , 16 );
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR , 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR , 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    
	// Open a window and create its OpenGL context
    
	GLFWmonitor* monitor = NULL;

    if(isFullScreen)
    {
        monitor = glfwGetPrimaryMonitor();

        renderSize = vec2(glfwGetVideoMode(monitor).width,glfwGetVideoMode(monitor).height);
    }
    
	window = glfwCreateWindow(renderSize.x, renderSize.y,windowTitle, monitor,NULL);
    
    
    glfwMakeContextCurrent(window);
    
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
	glfwSetCursorPos(window,renderSize.x/2, renderSize.y/2);
    
	//Neat grey background
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
    
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
    
	return window;
}