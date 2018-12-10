#include "RenderWindow.h"
using namespace BLAengine;

#ifdef GLFW_INTERFACE
#define GLFW_DEFAULT_WINDOW_NAME "glfwWindow"
void GLFWRenderWindow::CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen)
{
    GLFWwindow* window;
    // Initialise GLFW
    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        //Debug::OutputToDebug("Failed to initialize GLFW\n" );
        return;
    }
    m_isFullscreen = false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // Open a window and create its OpenGL context
    GLFWmonitor* monitor = nullptr;

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

    window = glfwCreateWindow(m_width, m_height, windowTitle.c_str(), monitor, NULL);

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile


    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initialize GLEW\n");
        //Debug::OutputToDebug("Failed to initialize GLEW\n");
        return;
    }

    glfwSwapInterval(0);

    glfwSetWindowTitle(window, windowTitle.c_str());

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(window, m_width / 2, m_height / 2);

    m_glfwWindow = window;
}

string GLFWRenderWindow::GetMaxGLVersion()
{
    return string("");
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

void GLFWRenderWindow::GetMouse(double & x, double& y)
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

GLFWRenderWindow::GLFWRenderWindow()
{
    m_isFullscreen = false;
    m_width = 0;
    m_height = 0;
    m_glfwWindow = nullptr;
}

GLFWRenderWindow::~GLFWRenderWindow()
{
    m_isFullscreen = false;
    m_width = 0;
    m_height = 0;
    m_glfwWindow = nullptr;
    glfwTerminate();
}

void GLFWRenderWindow::SetMouseCursorVisibility(bool visiblity)
{
	//GLFW_CURSOR_HIDDEN to not lock the cursor while hidden
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, visiblity ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

#endif

WPFRenderWindow::WPFRenderWindow() :
    m_glVersion(string("NONE")),
    m_makeGLCurrentRequest(true),
    m_updateWindowRequest(false),
    m_width(0), m_height(0),
    m_mousePosX(0), m_mousePosY(0),
    m_mouseDownState(0x00)
{
    for (int i = 0; i < 100; i++)
    {
        m_keyPressed[i] = false;
    }
}

void WPFRenderWindow::CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen)
{
    m_width = sizeX;
    m_height = sizeY;

    glewExperimental = GL_TRUE;
    glewInit();

    m_glVersion = string((char*)glGetString(GL_VERSION));
}

void WPFRenderWindow::UpdateWindowAndBuffers()
{
    m_updateWindowRequest = true;
}

void WPFRenderWindow::MakeGLContextCurrent()
{
    m_makeGLCurrentRequest = true;
}

void WPFRenderWindow::WriteSize(int x, int y)
{
    //std::cout << "Writing size: " << x << ", " << y << "\n";
    m_width = x;
    m_height = y;
}

void WPFRenderWindow::WriteMousePos(int x, int y)
{
    m_mousePosX = x;
    m_mousePosY = y;
}

void WPFRenderWindow::SetMouseXY() {}

bool WPFRenderWindow::GetKeyPressed(int key) 
{
    bool r = m_keyPressed[key];
    return r;
}

bool WPFRenderWindow::GetMousePressed(int button)
{ 
    int mask = (unsigned char) (0x01 << button);

    return (mask & m_mouseDownState) != 0x00;
}

string WPFRenderWindow::GetMaxGLVersion() { return m_glVersion; }

bool WPFRenderWindow::isFullScreen() 
{
    return false; 
}

void WPFRenderWindow::SetWindowTitle(string title) { }
string WPFRenderWindow::GetWindowTitle() { return ""; }

bool WPFRenderWindow::ShouldUpdateWindow() { return m_updateWindowRequest; }
void WPFRenderWindow::SetWindowUpdated() { m_updateWindowRequest = false; }

bool WPFRenderWindow::ShouldMakeGLCurrent() { return m_makeGLCurrentRequest; }
void WPFRenderWindow::SetMadeGLCurrent() { m_makeGLCurrentRequest = false; }

void WPFRenderWindow::GetSize(int &width, int &height)
{
    width = m_width;
    height = m_height;
}

void WPFRenderWindow::GetMouse(double &x, double &y)
{
    x = m_mousePosX;
    y = m_mousePosY;
}