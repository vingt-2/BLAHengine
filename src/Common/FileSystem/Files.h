#pragma once

#include <Common/System.h>

#include <string>
#include <vector>
namespace BLAengine
{
    typedef blaU64 BlaFileTime;
    typedef blaU64 BlaFileSize;

    struct FileEntry
    {
        std::string m_path;
        std::string m_name;
        std::string m_extention;

        BlaFileSize m_size;
        BlaFileTime m_lastEditTime;

        std::string GetFullPath() const { return m_path + m_name + m_extention; }
    };

    struct DirectoryEntry : FileEntry
    {
        enum EntryType : blaU32
        {
            DIRECTORY = 0,
            REGULAR_FILE
        } m_entryType;
    };

    void GetAllContentInDirectory(std::vector<DirectoryEntry> &directoryContent, const std::string &inDirectoryPath);

    void GetDirectoriesInDirectory(std::vector<DirectoryEntry> &directoryContent, const std::string &inDirectoryPath);

    void GetFilesInDirectory(std::vector<FileEntry> &directoryContent, const std::string &inDirectoryPath);

    std::string BlaFileTimeToString(BlaFileTime& blaFileTime);

    std::string BlaFileSizeToString(BlaFileSize& blaFileSize);

    std::string GetExecutablePath();

    std::string GetModulePath();

    std::string GetWorkingDir();

    FileEntry ParseFilePath(const std::string &filepath);
}
