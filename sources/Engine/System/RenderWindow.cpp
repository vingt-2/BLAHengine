#include "RenderWindow.h"

#define GLFW_DEFAULT_WINDOW_NAME "glfwWindow"

void GLFWRenderWindow::CreateWindow(int sizeX, int sizeY, bool isFullScreen)
{
	GLFWwindow* window;
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("Failed to initialize GLFW\n");
		//Debug::OutputToDebug("Failed to initialize GLFW\n" );
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// Open a window and create its OpenGL context
	GLFWmonitor* monitor = NULL;

	if (isFullScreen)
	{
		monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

		// Set window size to screen proportions
		m_width  = videoMode->width; 
		m_height = videoMode->height;

		m_isFullscreen = true;
	}
	else
	{
		m_width = sizeX;
		m_height = sizeY;
	}

	window = glfwCreateWindow(m_width, m_height, GLFW_DEFAULT_WINDOW_NAME, monitor, NULL);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile


	if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW\n");
		//Debug::OutputToDebug("Failed to initialize GLEW\n");
		return;
	}

	glfwSetWindowTitle(window, GLFW_DEFAULT_WINDOW_NAME);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, m_width / 2, m_height / 2);

	m_glfwWindow = window;
}

void GLFWRenderWindow::CreateOpenGLContext()
{
}

void GLFWRenderWindow::GetMaxGLVersion()
{
}

void GLFWRenderWindow::MakeGLContextCurrent()
{
	glfwMakeContextCurrent(m_glfwWindow);
}

void GLFWRenderWindow::UpdateWindowAndBuffers()
{
	glfwSwapInterval(0);

	glfwSwapBuffers(m_glfwWindow);
	glfwPollEvents();
}

void GLFWRenderWindow::GetSize(int &width, int &height)
{
	if (m_isFullscreen)
	{
		height = m_height;
		width = m_width;
	}
	else
	{
		glfwGetWindowSize(m_glfwWindow, &width, &height);
	}
}

bool GLFWRenderWindow::isFullScreen()
{
	return m_isFullscreen;
}

void GLFWRenderWindow::SetWindowTitle(std::string title)
{
}

std::string GLFWRenderWindow::GetWindowTitle()
{
	return std::string();
}

void GLFWRenderWindow::GetMouse(double & x, double & y)
{
	glfwGetCursorPos(m_glfwWindow, &x, &y);
}

void GLFWRenderWindow::SetMouseXY()
{
}

bool GLFWRenderWindow::GetKeyPressed(int key)
{
	return (glfwGetKey(m_glfwWindow, key) == GLFW_PRESS);
}

bool GLFWRenderWindow::GetMousePressed(int button)
{
	return (glfwGetMouseButton(m_glfwWindow, button) == GLFW_PRESS);
}
