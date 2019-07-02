#pragma once

#include <Common/System.h>

#include <Common/StdInclude.h>

namespace BLAengine
{
    typedef blaU64 BlaFileTime;
    typedef blaU64 BlaFileSize;

    struct FileEntry
    {
        blaString m_path;
        blaString m_name;
        blaString m_extention;

        BlaFileSize m_size;
        BlaFileTime m_lastEditTime;

        blaString GetFullPath() const { return m_path + m_name + m_extention; }
    };

    struct DirectoryEntry : FileEntry
    {
        enum EntryType : blaU32
        {
            DIRECTORY = 0,
            REGULAR_FILE
        } m_entryType;
    };

    void GetAllContentInDirectory(blaVector<DirectoryEntry> &directoryContent, const blaString &inDirectoryPath);

    void GetDirectoriesInDirectory(blaVector<DirectoryEntry> &directoryContent, const blaString &inDirectoryPath);

    void GetFilesInDirectory(blaVector<FileEntry> &directoryContent, const blaString &inDirectoryPath);

    blaString BlaFileTimeToString(BlaFileTime& blaFileTime);

    blaString BlaFileSizeToString(BlaFileSize& blaFileSize);

    blaString GetExecutablePath();

    blaString GetModulePath();

    blaString GetWorkingDir();

    FileEntry ParseFilePath(const blaString &filepath);
}
