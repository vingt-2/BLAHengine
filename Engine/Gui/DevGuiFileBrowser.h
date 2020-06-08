// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"
#include "System/FileSystem/Files.h"

#include "DevGuiWindow.h"

#include "StdInclude.h"

struct GLFWwindow;

namespace BLA
{
    class BlaFileBrowser
    {
        friend class DevGuiManager;
    public:

        enum FileBrowserState
        {
            BROWSING_FIRST_RENDER,
            BROWSING,
            CONFIRMED_SELECTION,
            CANCELLED
        };

        BLACORE_API BlaFileBrowser(blaString browserName,
                       blaString filesDirectory,
                       blaString directoryDirectory);
    	
        BLACORE_API blaBool IsBrowsingCancelled() const;
    	
        virtual void Render();
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
        BLACORE_API OpenFilePrompt(blaString browserName,
	                   blaString filesDirectory,
	                   blaString directoryDirectory,
	                   blaBool disableMultipleSelection);
    	
        BLACORE_API blaBool GetConfirmedSelection(blaVector<FileEntry>& selection) const;
    	
        void Render() override;
    };

    class SaveFilePrompt : public BlaFileBrowser
    {
    public:
        BLACORE_API SaveFilePrompt(blaString browserName,
	                   blaString filesDirectory,
	                   blaString directoryDirectory);
        BLACORE_API blaBool GetConfirmedSavePath(blaString& savePath) const;
    	
        void Render() override;

    private:

        blaString m_currentSavePath;
    };
}