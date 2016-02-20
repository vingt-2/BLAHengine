//#include "OSXGL32_Renderer.h"
//
//OSXGL32Renderer::OSXGL32Renderer(char* windowTitle, bool isFullScreen):
//Renderer(windowTitle,isFullScreen)
//{
//	glfwWindow = InitializeContext(windowTitle);
//	if(glfwWindow != NULL)
//	{
//		isContextEnabled = true;
//	}
//}
//
//OSXGL32Renderer::~OSXGL32Renderer()
//{
//}
//
//bool OSXGL32Renderer::Update()
//{
//	//Set OpenGL to this context.
//	glfwMakeContextCurrent(glfwWindow);
//	
//	//Adjust Projection and ViewPort.
//	m_mainCamera->SetProjection(glm::frustum(-0.0001f*renderSize.x, 0.0001f*renderSize.x,-0.0001f*renderSize.y, 0.0001f*renderSize.y, 0.1f, 100.0f));
//	glViewport(0,0,renderSize.x,renderSize.y);
//	
//	// Clear Screen Buffer
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	
//	//Render Scene Objects.
//	
//	
//	// Enable Z-Buffer test.
//	glEnable(GL_DEPTH_TEST);
//	for(int i = 0; i < m_renderPool.size();i++)
//	{
//		m_renderPool[i]->Draw(m_mainCamera->m_projection, m_mainCamera->m_viewTransform.transformMatrix);
//	}
//	
//	// Render m_debug gizmos
//	
//	// Disable Z-Buffer test for m_debug gizmos.
//	glDisable(GL_DEPTH_TEST);
//	
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	
//	for (int i = 0; i < m_debug->m_gizmoVector.size(); i++)
//	{
//		m_debug->m_gizmoVector[i]->Draw(m_mainCamera->m_projection, m_mainCamera->m_viewTransform.transformMatrix);
//	}
//	for (int i = 0; i < m_debug->m_gizmoVector.size(); i++)
//	{
//		m_debug->m_gizmoVector[i]->~MeshRenderer();
//	}
//	
//	m_debug->m_gizmoVector.clear();
//	glDisable(GL_BLEND);
//	
//	glfwSwapInterval(1);
//	
//	glfwSwapBuffers(glfwWindow);
//	glfwPollEvents();
//	
//	return true;
//}
//
//
//GLFWwindow* OSXGL32Renderer::InitializeContext(char* windowTitle)
//{
//	
//	GLFWwindow* window;
//	// Initialise GLFW
//	if( !glfwInit() )
//	{
//		glfwTerminate();
//		//Debug::OutputToDebug("Failed to initialize GLFW\n" );
//		exit(EXIT_FAILURE);
//	}
//	
//	//glfwWindowHint(GLFW_SAMPLES , 16 );
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR , 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR , 2);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	
//	// Open a window and create its OpenGL context
//
//	GLFWmonitor* monitor = NULL;
//	
//	
//	if(isFullScreen)
//	{
//		monitor = glfwGetPrimaryMonitor();
//
//		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
//
//		renderSize = vec2(videoMode->width, videoMode->height);
//	}
//	
//	window = glfwCreateWindow(renderSize.x, renderSize.y,windowTitle, monitor,NULL);
//	
//	if (!window)
//	{
//		glfwTerminate();
//		exit(EXIT_FAILURE);
//	}
//	
//	glfwMakeContextCurrent(window);
//	
//	glfwSetWindowTitle(window, windowTitle);
//	
//	// Ensure we can capture the escape key being pressed below
//	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
//	glfwSetCursorPos(window,renderSize.x/2, renderSize.y/2);
//	
//	//Neat grey background
//	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
//	
//	// Accept fragment if it closer to the camera than the former one
//	glDepthFunc(GL_LESS);
//	
//	// Cull triangles which normal is not towards the camera
//	glEnable(GL_CULL_FACE);
//	
//	return window;
//}