#pragma once

#include "Maths/Maths.h"
#include "BLASingleton.h"

#include "./GuiWindow.h"
#include "./GuiMenu.h"
#include "./GuiFileBrowser.h"

#include "StdInclude.h"

struct GLFWwindow;

namespace BLA
{
    class BLACORE_API BlaGuiManager
    {
        BLA_DECLARE_SINGLETON(BlaGuiManager)

    public:
        BlaGuiManager(GLFWwindow* glfwWindow) :
            m_window(glfwWindow)
        {
            m_showDockspace = true;
            Init();
            m_lastFileBrowserOpenDirectory = "./";
        }

        ~BlaGuiManager()
        {
            Destroy();
        }

        void Update();

        blaBool IsMouseOverGui() const;

        void DrawText(const blaString& textToDraw, blaIVec2 renderWindowPosition);

        void OpenConsole(const blaString& consoleName);

        OpenFilePrompt* CreateOpenFilePrompt(blaString browserName, blaBool disableMultipleSelection);

        SaveFilePrompt* CreateSaveFilePrompt(blaString browserName);

        blaBool CloseFileBrowser(blaString browserName);

        BlaGuiMenu m_menuBar;

        BlaGuiWindow* OpenWindow(blaString name);

        BlaGuiWindow* GetWindow(blaString name);

        void OpenWindow(blaString name, BlaGuiWindow* window);

    private:

        void Init();
        void Destroy();

        blaMap<blaString, BlaGuiWindow*> m_openWindows;

        blaMap<blaString, BlaFileBrowser*> m_openBrowsers;

        blaString m_lastFileBrowserOpenDirectory;

        GLFWwindow* m_window;

        bool m_showDockspace;
    };
}