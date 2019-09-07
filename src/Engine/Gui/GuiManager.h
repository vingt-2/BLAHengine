#pragma once

#include <Common/Maths/Maths.h>
#include <Common/BLASingleton.h>

#include "./GuiWindow.h"
#include "./GuiMenu.h"
#include "./GuiFileBrowser.h"

#include <Common/StdInclude.h>

struct GLFWwindow;

namespace BLAengine
{
    class BlaGuiManager
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

    public:

        void Update();

        blaBool IsMouseOverGui() const;

        void DrawText(const blaString& textToDraw, blaIVec2 renderWindowPosition);

        void OpenConsole(const blaString& consoleName);

        OpenFilePrompt* CreateOpenFilePrompt(blaString browserName, blaBool disableMultipleSelection);
        
        SaveFilePrompt* CreateSaveFilePrompt(blaString browserName);

        blaBool CloseFileBrowser(blaString browserName);

        BlaGuiMenu m_menuBar;

		BlaGuiWindow* OpenWindow(blaString name);

		void OpenWindow(blaString name, BlaGuiWindow* window);

		const BlaGuiWindow* GetWindow(const blaString& name) const { return m_openWindows.at(name); }

    private:

        void Init();
        void Destroy();

        blaVector<BlaGuiWindow> m_oneTimeWindows;

        blaMap<blaString, BlaGuiWindow*> m_openWindows;

        blaMap<blaString, BlaFileBrowser*> m_openBrowsers;

        blaString m_lastFileBrowserOpenDirectory;

        GLFWwindow* m_window;

		bool m_showDockspace;
    };
}