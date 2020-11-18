// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"
#include "BLASingleton.h"

#include "DevGuiWindow.h"
#include "DevGuiMenu.h"
#include "DevGuiFileBrowser.h"

#include "StdInclude.h"

struct GLFWwindow;

namespace BLA
{
    class RenderWindow;
    class BLACORE_API DevGuiManager
    {
        BLA_DECLARE_SINGLETON(DevGuiManager)

    public:
        DevGuiManager(RenderWindow* glfwWindow);
        ~DevGuiManager();

        void Update(bool editorBuild);

        blaBool IsMouseOverGui() const;

        void DrawText(const blaString& textToDraw, blaIVec2 renderWindowPosition);

        void OpenConsole(const blaString& consoleName);

        OpenFilePrompt* CreateOpenFilePrompt(blaString browserName, blaBool disableMultipleSelection);

        SaveFilePrompt* CreateSaveFilePrompt(blaString browserName);

        blaBool CloseFileBrowser(blaString browserName);

        DevGuiMenu m_menuBar;

        DevGuiWindow* OpenWindow(blaString name);

        DevGuiWindow* GetWindow(blaString name);

        void OpenWindow(blaString name, DevGuiWindow* window);

    private:

        void Init();
        void Destroy();

        blaMap<blaString, DevGuiWindow*> m_openWindows;

        blaMap<blaString, BlaFileBrowser*> m_openBrowsers;

        blaString m_lastFileBrowserOpenDirectory;

        RenderWindow* m_window;
    };
}