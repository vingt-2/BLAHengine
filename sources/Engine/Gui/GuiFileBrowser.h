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
            CANCELLED
        };

        BlaFileBrowser(std::string browserName, 
            std::string filesDirectory, 
            std::string directoryDirectory):
              m_name(browserName)
            , m_currentFilesDirectory(filesDirectory)
            , m_currentDirectoriesDirectory(directoryDirectory)
            , m_currentState(BROWSING_FIRST_RENDER)
        {};

        virtual void Render();

        blaBool IsBrowsingCancelled() const;
    protected:

        void FileBrowserDisplayAllContentNonRecursive();

        void FileBrowserDisplayDirectoriesRecursive(std::string currentdirectory, blaBool displayEditTime);

        void CurrentFolderGoBack();

        std::string m_currentFilesDirectory;
        std::string m_currentDirectoriesDirectory;
        std::string m_name;

        std::unordered_map<std::string, FileEntry> m_currentSelection;

        FileBrowserState m_currentState;

        blaBool m_disableMultipleSelection;
        //void FileBrowserDisplayAllContentRecursive();
    };

    class OpenFilePrompt : public BlaFileBrowser
    {
    public:
        OpenFilePrompt(std::string browserName,
            std::string filesDirectory,
            std::string directoryDirectory,
            blaBool disableMultipleSelection):
          BlaFileBrowser(browserName, filesDirectory, directoryDirectory)
        {
            m_disableMultipleSelection = disableMultipleSelection;
        }

        void Render() override;

        blaBool GetConfirmedSelection(std::vector<FileEntry>& selection) const;
    private:
    };

    class SaveFilePrompt : public BlaFileBrowser
    {
    public:
        SaveFilePrompt(std::string browserName,
            std::string filesDirectory,
            std::string directoryDirectory) :
            BlaFileBrowser(browserName, filesDirectory, directoryDirectory)
        {
            m_disableMultipleSelection = true;
        }

        void Render() override;

        blaBool GetConfirmedSavePath(std::string& savePath) const;
    private:

        std::string m_currentSavePath;
    };
}