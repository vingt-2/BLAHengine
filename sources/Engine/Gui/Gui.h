#pragma once

struct GLFWwindow;

namespace BLAengine
{

    class GuiTest
    {
    public:
        GuiTest(GLFWwindow* glfwWindow) :
            m_window(glfwWindow)
        {
            Init();
        }

        ~GuiTest()
        {
            Destroy();
        }

    public:

        void Update();

    private:

        void Init();
        void Destroy();

        GLFWwindow* m_window;
    };
}