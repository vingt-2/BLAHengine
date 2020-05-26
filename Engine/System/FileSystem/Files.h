// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"

#include "StdInclude.h"

namespace BLA
{
    typedef blaU64 BlaFileTime;
    typedef blaU64 BlaFileSize;

    struct FileEntry
    {
        blaString m_path;
        blaString m_name;
        blaString m_extension;

        BlaFileSize m_size;
        BlaFileTime m_lastEditTime;

        blaString GetFullPath() const { return m_path + m_name + m_extension; }
    };

    struct DirectoryEntry : FileEntry
    {
        enum EntryType : blaU32
        {
            DIRECTORY = 0,
            REGULAR_FILE
        } m_entryType;
    };

    void BLACORE_API GetAllContentInDirectory(blaVector<DirectoryEntry> &directoryContent, const blaString &inDirectoryPath);

    void BLACORE_API GetDirectoriesInDirectory(blaVector<DirectoryEntry> &directoryContent, const blaString &inDirectoryPath);

    void BLACORE_API GetFilesInDirectory(blaVector<FileEntry> &directoryContent, const blaString &inDirectoryPath);

    blaString BLACORE_API BlaFileTimeToString(BlaFileTime& blaFileTime);

    blaString BLACORE_API BlaFileSizeToString(BlaFileSize& blaFileSize);

    blaString BLACORE_API GetExecutablePath();

    blaString BLACORE_API GetModulePath();

    blaString BLACORE_API GetWorkingDir();

    FileEntry BLACORE_API ParseFilePath(const blaString &filepath);
}
