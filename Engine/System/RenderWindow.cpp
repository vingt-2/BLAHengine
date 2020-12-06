// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RenderWindow.h"
#include "InputManager.h"
#include "Time.h"
#include <GLFW/glfw3.h>

using namespace BLA;

blaF32 System::GetSystemTime()
{
    return (blaF32)glfwGetTime();
}

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

namespace BLA
{
    void GLFWKeyboardCallBack(GLFWwindow* window, int keyCode, int scandone, int action, int mods)
    {
        if (action == GLFW_REPEAT)
        {
            return;
        }

        BLAKeyboard key = GLFWToBlaKeyboard(keyCode);

        if (key != BLA_KEY_ENUM_END)
        {
            InputStateSetter::SetKey(GLFWToBlaKeyboard(keyCode), (float)glfwGetTime(), action == GLFW_PRESS);
        }
    }

    void GLFWMouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
    {
        if (action == GLFW_REPEAT)
        {
            return;
        }

        InputStateSetter::SetMouseButton((BLAMouseButtons)button, (float)glfwGetTime(), action == GLFW_PRESS);
    }

    void GLFWMouseCursorPosCallBack(GLFWwindow* window, double xpos, double ypos)
    {
        InputStateSetter::SetMousePointer(blaVec2(xpos, ypos));
    }

    void GLFWMouseWheelCallback(GLFWwindow* window, double xAxisScroll, double yAxisScroll)
    {
        float yScroll = (float)yAxisScroll;
        InputStateSetter::SetMouseScrollDelta(yScroll);
    }
}

void GLFWRenderWindow::InitGLFW()
{
    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        return;
    }
}

void GLFWRenderWindow::ShutdownGLFW()
{
    glfwTerminate();
}

void GLFWRenderWindow::UpdateWindowAndBuffers()
{
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

GLFWRenderWindow::GLFWRenderWindow() :
    m_isFullscreen(false),
    m_width(0),
    m_height(0),
    m_glfwWindow(nullptr),
    m_registeredDragAndDropCallback(nullptr),
    m_cursorVisibility(true)
{};

GLFWRenderWindow::~GLFWRenderWindow()
{
    glfwDestroyWindow(m_glfwWindow);
}

void GLFWRenderWindow::SetMouseCursorVisibility(bool visiblity)
{
    //GLFW_CURSOR_HIDDEN to not lock the cursor while hidden
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, visiblity ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    m_cursorVisibility = visiblity;
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