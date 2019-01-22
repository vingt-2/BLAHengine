#pragma once

#include <Common/Maths/Maths.h>
#include "Common/FileSystem/Files.h"

#include "./GuiWindow.h"
#include "./GuiMenu.h"

#include <string>
#include <vector>
#include <unordered_map>

struct GLFWwindow;

namespace BLAengine
{
    class BlaFileBrowser
    {
        friend class BlaGuiManager;
    public:

        enum FileBrowserState
        {
            BROWSING_FIRST_RENDER,
            BROWSING,
            CONFIRMED_SELECTION,
            CANCELLED_SELECTION
        };

        BlaFileBrowser(std::string browserName, 
            std::string filesDirectory, 
            std::string directoryDirectory, 
            const blaIVec2& windowPosition,
            blaBool disableMultipleSelection) :
            m_name(browserName)
            , m_currentFilesDirectory(filesDirectory)
            , m_currentDirectoriesDirectory(directoryDirectory)
            , m_currentState(BROWSING_FIRST_RENDER)
            , m_disableMultipleSelection(disableMultipleSelection)
        {};

        void Render();

        blaBool GetConfirmedSelection(std::vector<FileEntry>& selection) const;
        blaBool GetSelectionCancelled() const;
    private:

        void FileBrowserDisplayAllContentRecursive();

        void FileBrowserDisplayAllContentNonRecursive();

        void FileBrowserDisplayDirectoriesRecursive(std::string currentdirectory, blaBool displayEditTime);

        std::string m_currentFilesDirectory;
        std::string m_currentDirectoriesDirectory;
        std::string m_name;

        std::unordered_map<std::string, FileEntry> m_currentSelection;

        FileBrowserState m_currentState;

        blaBool m_disableMultipleSelection;
    };
}