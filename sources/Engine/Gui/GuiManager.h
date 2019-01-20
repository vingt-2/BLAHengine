#pragma once

#include <Common/Maths/Maths.h>

#include "./GuiWindow.h"
#include "./GuiMenu.h"

#include <string>
#include <vector>
#include <unordered_map>

struct GLFWwindow;

namespace BLAengine
{
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

        blaBool IsMouseOverGui() const;

        void DrawText(std::string textToDraw, blaIVec2 renderWindowPosition);

        void FileBrowser(const std::string startingDirectory, std::string& selection);

        BlaGuiMenu m_menuBar;

    private:

        void Init();
        void Destroy();

        std::vector<BlaGuiWindow> m_oneTimeWindows;

        std::unordered_map<std::string, BlaGuiWindow> m_permanentWindows;

        GLFWwindow* m_window;
    };
}