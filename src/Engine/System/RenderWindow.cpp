#include "RenderWindow.h"
#include "InputManager.h"

using namespace BLAengine;

#ifdef GLFW_INTERFACE

blaVector<GLFWRenderWindow*> GLFWRenderWindow::ms_glfwRenderWindowInstanced = blaVector<GLFWRenderWindow*>();

void GLFWRenderWindow::GLFWDragAndDropCallBack(GLFWwindow* glfwWindow, int argc, char** paths)
{
    GLFWRenderWindow* renderWindow = nullptr;
    for (int i = 0; i < ms_glfwRenderWindowInstanced.size(); ++i)
    {
        if (ms_glfwRenderWindowInstanced[i]->GetWindowPointer() == glfwWindow)
        {
            renderWindow = ms_glfwRenderWindowInstanced[i];
        }
    }

    if (renderWindow)
    {
        if (renderWindow->m_registeredDragAndDropCallback != nullptr)
        {
            DragAndDropPayloadDontStore dragAndDropPayload;
            for (int i = 0; i < argc; ++i)
            {
                dragAndDropPayload.push_back(blaString(paths[i]));
            }
            renderWindow->m_registeredDragAndDropCallback(&dragAndDropPayload);
        }
    }
}

void GLFWMouseWheelCallback(GLFWwindow* window, double xAxisScroll, double yAxisScroll)
{
    InputStateSetter::SetMouseScrollDelta(yAxisScroll);
}

BLAKeyboard GLFWToBlaKeyboard(int glfwKey)
{
    if (glfwKey >= GLFW_KEY_A && glfwKey <= GLFW_KEY_RIGHT_BRACKET)
    {
        return (BLAKeyboard)(BLA_KEY_A + (glfwKey - GLFW_KEY_A));
    }

    if (glfwKey >= GLFW_KEY_COMMA && glfwKey <= GLFW_KEY_9)
    {
        return (BLAKeyboard)(BLA_KEY_COMMA + (glfwKey - GLFW_KEY_COMMA));
    }

    if (glfwKey >= GLFW_KEY_ESCAPE && glfwKey <= GLFW_KEY_END)
    {
        return (BLAKeyboard)(BLA_KEY_ESCAPE + (glfwKey - GLFW_KEY_ESCAPE));
    }

    if (glfwKey >= GLFW_KEY_CAPS_LOCK && glfwKey <= GLFW_KEY_PAUSE)
    {
        return (BLAKeyboard)(BLA_KEY_CAPS_LOCK + (glfwKey - GLFW_KEY_CAPS_LOCK));
    }

    if (glfwKey >= GLFW_KEY_F1 && glfwKey <= GLFW_KEY_F25)
    {
        return (BLAKeyboard)(BLA_KEY_F1 + (glfwKey - GLFW_KEY_F1));
    }

    if (glfwKey >= GLFW_KEY_KP_0 && glfwKey <= GLFW_KEY_KP_EQUAL)
    {
        return (BLAKeyboard)(BLA_KEY_KP_0 + (glfwKey - GLFW_KEY_KP_0));
    }

    if (glfwKey >= GLFW_KEY_LEFT_SHIFT && glfwKey <= GLFW_KEY_MENU)
    {
        return (BLAKeyboard)(BLA_KEY_LEFT_SHIFT + (glfwKey - GLFW_KEY_LEFT_SHIFT));
    }

    if (glfwKey == GLFW_KEY_SPACE) return BLA_KEY_SPACE;

    if (glfwKey == GLFW_KEY_APOSTROPHE) return BLA_KEY_APOSTROPHE;

    if (glfwKey == GLFW_KEY_SEMICOLON) return BLA_KEY_SEMICOLON;

    if (glfwKey == GLFW_KEY_GRAVE_ACCENT) return BLA_KEY_GRAVE_ACCENT;

    if (glfwKey == GLFW_KEY_WORLD_1) return BLA_KEY_WORLD_1;

    if (glfwKey == GLFW_KEY_WORLD_2) return BLA_KEY_WORLD_2;

    return BLA_KEY_ENUM_END;
}

void GLFWKeyboardCallBack(GLFWwindow* window, int keyCode, int scandone, int action, int mods)
{
    if (action == GLFW_REPEAT)
    {
        return;
    }

    BLAKeyboard key = GLFWToBlaKeyboard(keyCode);

    if (key != BLA_KEY_ENUM_END)
    {
        InputStateSetter::SetKey(GLFWToBlaKeyboard(keyCode), glfwGetTime(), action == GLFW_PRESS);
    }
}

void GLFWMouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_REPEAT)
    {
        return;
    }

    InputStateSetter::SetMouseButton((BLAMouseButtons) button, glfwGetTime(), action == GLFW_PRESS);
}

void GLFWMouseCursorPosCallBack(GLFWwindow* window, double xpos, double ypos)
{
    InputStateSetter::SetMousePointer(blaVec2(xpos, ypos));
}

void GLFWRenderWindow::CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen)
{
    GLFWwindow* window;
    // Initialise GLFW
    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        return;
    }

    m_isFullscreen = false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    //glfwWindowHint(GLFW_DECORATED, false);

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

    ms_glfwRenderWindowInstanced.push_back(this);

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile


    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initialize GLEW\n");
        //DebugDraw::OutputToDebug("Failed to initialize GLEW\n");
        return;
    }

    glfwSwapInterval(1);

    glfwSetWindowTitle(window, windowTitle.c_str());

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(window, m_width / 2, m_height / 2);

    glfwSetMouseButtonCallback(window, GLFWMouseButtonCallBack);
    glfwSetCursorPosCallback(window, GLFWMouseCursorPosCallBack);
    glfwSetKeyCallback(window, GLFWKeyboardCallBack);
    glfwSetScrollCallback(window, (GLFWscrollfun) GLFWMouseWheelCallback);
    
    glfwSetDropCallback(window, (GLFWdropfun)GLFWDragAndDropCallBack);

    m_glfwWindow = window;
}

blaString GLFWRenderWindow::GetMaxGLVersion() const
{
    return blaString("");
}

void GLFWRenderWindow::MakeGLContextCurrent()
{
    glfwMakeContextCurrent(m_glfwWindow);
}

void GLFWRenderWindow::UpdateWindowAndBuffers()
{
    bool cursorVisibility = true;
    for (auto mouseButton : m_mouseButtonsThatKillCursorWhenHeld)
    {
        if (glfwGetMouseButton(m_glfwWindow, mouseButton) == GLFW_PRESS)
        {
            cursorVisibility = false;
            break;
        }
    }
    SetMouseCursorVisibility(cursorVisibility);
    
    glfwSwapInterval(0);

    glfwSwapBuffers(m_glfwWindow);
    glfwPollEvents();

    glfwGetWindowSize(m_glfwWindow, &m_width, &m_height);
}

void GLFWRenderWindow::GetSize(int &width, int &height) const
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

bool GLFWRenderWindow::isFullScreen() const
{
    return m_isFullscreen;
}

void GLFWRenderWindow::SetWindowTitle(blaString title)
{
    glfwSetWindowTitle(m_glfwWindow, title.c_str());
}

blaString GLFWRenderWindow::GetWindowTitle() const
{
    return blaString();
}

GLFWRenderWindow::GLFWRenderWindow():
    m_isFullscreen(false),
    m_width(0),
    m_height(0),
    m_glfwWindow(nullptr),
    m_registeredDragAndDropCallback(nullptr)
{};

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

void GLFWRenderWindow::SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton)
{
    m_mouseButtonsThatKillCursorWhenHeld.push_back(mouseButton);
}

blaVec2 GLFWRenderWindow::GetMousePointerScreenSpaceCoordinates()
{
    double x, y;
    glfwGetCursorPos(m_glfwWindow, &x, &y);

    return blaVec2((m_width - x) / m_width, (m_height - y) / m_height);
}

blaIVec2 GLFWRenderWindow::GetMousePositionInWindow()
{
    double x, y;
    glfwGetCursorPos(m_glfwWindow, &x, &y);

    return blaIVec2((blaU32)x, (blaU32)y);
}

void GLFWRenderWindow::SetDragAndDropCallback(DragAndDropCallback dragAndDropCallback)
{
    m_registeredDragAndDropCallback = dragAndDropCallback;
}


#elif defined(WPF_INTERFACE)

WPFRenderWindow::WPFRenderWindow() :
    m_glVersion(blaString("NONE")),
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

void WPFRenderWindow::CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen)
{
    m_width = sizeX;
    m_height = sizeY;

    glewExperimental = GL_TRUE;
    glewInit();

    m_glVersion = blaString((char*)glGetString(GL_VERSION));
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

//void WPFRenderWindow::SetMouseXY() {}
//
//bool WPFRenderWindow::GetKeyPressed(int key) const
//{
//    bool r = m_keyPressed[key];
//    return r;
//}
//
//bool WPFRenderWindow::GetMousePressed(int button) const
//{ 
//    int mask = (unsigned char) (0x01 << button);
//
//    return (mask & m_mouseDownState) != 0x00;
//}

blaString WPFRenderWindow::GetMaxGLVersion() const { return m_glVersion; }

bool WPFRenderWindow::isFullScreen() const
{
    return false; 
}

void WPFRenderWindow::SetWindowTitle(blaString title) { }
blaString WPFRenderWindow::GetWindowTitle() const { return ""; }

bool WPFRenderWindow::ShouldUpdateWindow() const { return m_updateWindowRequest; }
void WPFRenderWindow::SetWindowUpdated() { m_updateWindowRequest = false; }

bool WPFRenderWindow::ShouldMakeGLCurrent() const { return m_makeGLCurrentRequest; }
void WPFRenderWindow::SetMadeGLCurrent() { m_makeGLCurrentRequest = false; }

void WPFRenderWindow::GetSize(int &width, int &height) const
{
    width = m_width;
    height = m_height;
}
//
//void WPFRenderWindow::GetMouse(double &x, double &y) const
//{
//    x = m_mousePosX;
//    y = m_mousePosY;
//}
#endif