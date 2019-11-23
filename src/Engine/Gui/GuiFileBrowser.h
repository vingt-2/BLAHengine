#pragma once

#include <Common/Maths/Maths.h>
#include "Common/FileSystem/Files.h"

#include "./GuiWindow.h"
#include "./GuiMenu.h"

#include <Common/StdInclude.h>

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

        BlaFileBrowser(blaString browserName,
            blaString filesDirectory,
            blaString directoryDirectory) :
            m_name(browserName)
            , m_currentFilesDirectory(filesDirectory)
            , m_currentDirectoriesDirectory(directoryDirectory)
            , m_currentState(BROWSING_FIRST_RENDER)
        {};

        virtual void Render();

        blaBool IsBrowsingCancelled() const;
    protected:

        void FileBrowserDisplayAllContentNonRecursive();

        void FileBrowserDisplayDirectoriesRecursive(blaString currentdirectory, blaBool displayEditTime);

        void CurrentFolderGoBack();

        blaString m_currentFilesDirectory;
        blaString m_currentDirectoriesDirectory;
        blaString m_name;

        blaMap<blaString, FileEntry> m_currentSelection;

        FileBrowserState m_currentState;

        blaBool m_disableMultipleSelection;
        //void FileBrowserDisplayAllContentRecursive();
    };

    class OpenFilePrompt : public BlaFileBrowser
    {
    public:
        OpenFilePrompt(blaString browserName,
            blaString filesDirectory,
            blaString directoryDirectory,
            blaBool disableMultipleSelection) :
            BlaFileBrowser(browserName, filesDirectory, directoryDirectory)
        {
            m_disableMultipleSelection = disableMultipleSelection;
        }

        void Render() override;

        blaBool GetConfirmedSelection(blaVector<FileEntry>& selection) const;
    private:
    };

    class SaveFilePrompt : public BlaFileBrowser
    {
    public:
        SaveFilePrompt(blaString browserName,
            blaString filesDirectory,
            blaString directoryDirectory) :
            BlaFileBrowser(browserName, filesDirectory, directoryDirectory)
        {
            m_disableMultipleSelection = true;
        }

        void Render() override;

        blaBool GetConfirmedSavePath(blaString& savePath) const;
    private:

        blaString m_currentSavePath;
    };
}