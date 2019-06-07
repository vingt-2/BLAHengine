#pragma once

#include <Common/Maths/Maths.h>

#include "./GuiWindow.h"
#include "./GuiMenu.h"
#include "./GuiFileBrowser.h"

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
            m_lastFileBrowserOpenDirectory = "./";
        }

        ~BlaGuiManager()
        {
            Destroy();
        }

    public:

        void Update();

        blaBool IsMouseOverGui() const;

        void DrawText(std::string textToDraw, blaIVec2 renderWindowPosition);

        OpenFilePrompt* CreateOpenFilePrompt(std::string browserName, blaBool disableMultipleSelection);
        
        SaveFilePrompt* CreateSaveFilePrompt(std::string browserName);

        blaBool CloseFileBrowser(std::string browserName);

        BlaGuiMenu m_menuBar;

    private:

        void Init();
        void Destroy();

        std::vector<BlaGuiWindow> m_oneTimeWindows;

        std::unordered_map<std::string, BlaGuiWindow> m_openWindows;

        std::unordered_map<std::string, BlaFileBrowser*> m_openBrowsers;

        std::string m_lastFileBrowserOpenDirectory;

        GLFWwindow* m_window;
    };
}