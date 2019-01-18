#pragma once

#include <Common/Maths/Maths.h>
#include <Common/DataStructures/Tree.h>

#include <string>
#include <vector>

struct GLFWwindow;

namespace BLAengine
{
    class BlaGuiWindow;

    class BlaGuiManager
    {
    public:
        BlaGuiManager(GLFWwindow* glfwWindow) :
            m_window(glfwWindow)
        {
            Init();
        }

        ~BlaGuiManager()
        {
            Destroy();
        }

    public:

        void Update();

        void DrawText(std::string textToDraw, blaIVec2 renderWindowPosition);

    private:

        void Init();
        void Destroy();

        std::vector<BlaGuiWindow*> m_imguiWindows;

        GLFWwindow* m_window;
    };
}