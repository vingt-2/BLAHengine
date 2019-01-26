#include "Files.h"

#include <External/Files/cute_files.h>

#include <iomanip>
#include <sstream>
#include "External/Files/dirent.h"

using namespace BLAengine;

void BLAengine::GetAllContentInDirectory(std::vector<DirectoryEntry>& directoryContent, const std::string& inDirectoryPath)
{
    cf_dir_t dir;
    cf_dir_open(&dir, inDirectoryPath.c_str());

    while (dir.has_next)
    {
        cf_file_t file;
        cf_read_file(&dir, &file);

        DirectoryEntry entry;
        if (file.is_dir && strcmp(file.name, "."))
        {
            entry.m_entryType = DirectoryEntry::DIRECTORY;
            entry.m_path = file.path;
            entry.m_name = file.name;
            cf_get_file_time(file.path, (cf_time_t*)&(entry.m_lastEditTime));
            directoryContent.push_back(entry);
        }
        else if (file.is_reg)
        {
            entry.m_entryType = DirectoryEntry::REGULAR_FILE;
            entry.m_name = file.name;
            const size_t posExt = entry.m_name.find_first_of('.');
            if (posExt != std::string::npos)
            {
                entry.m_extention = entry.m_name.substr(posExt);
                entry.m_name = entry.m_name.substr(0, posExt);
            }
            entry.m_size = file.size;
            entry.m_path = file.path;
            cf_get_file_time(file.path, (cf_time_t*)&(entry.m_lastEditTime));
            directoryContent.push_back(entry);
        }

        cf_dir_next(&dir);
    }

    cf_dir_close(&dir);
}

void BLAengine::GetDirectoriesInDirectory(std::vector<DirectoryEntry>& directoryContent, const std::string& inDirectoryPath)
{
    cf_dir_t dir;
    cf_dir_open(&dir, inDirectoryPath.c_str());

    while (dir.has_next)
    {
        cf_file_t file;
        cf_read_file(&dir, &file);

        DirectoryEntry entry;
        
        if (file.is_dir && file.name[0] != '.')
        {
            entry.m_entryType = DirectoryEntry::DIRECTORY;
            entry.m_path = file.path;
            entry.m_name = file.name;
            cf_get_file_time(file.path, (cf_time_t*)&(entry.m_lastEditTime));
            directoryContent.push_back(entry);
        }

          cf_dir_next(&dir);
    }

    cf_dir_close(&dir);
}

void BLAengine::GetFilesInDirectory(std::vector<FileEntry>& directoryContent, const std::string& inDirectoryPath)
{
    cf_dir_t dir;
    cf_dir_open(&dir, inDirectoryPath.c_str());

    while (dir.has_next)
    {
        cf_file_t file;
        cf_read_file(&dir, &file);

        FileEntry entry;
        
        if (file.is_reg)
        {
            entry.m_name = file.name;
            const size_t posExt = entry.m_name.find_first_of('.');
            if (posExt != std::string::npos)
            {
                entry.m_extention = entry.m_name.substr(posExt);
                entry.m_name = entry.m_name.substr(0, posExt);
            }
            entry.m_size = file.size;
            entry.m_path = file.path;
            cf_get_file_time(file.path, (cf_time_t*)&(entry.m_lastEditTime));
            directoryContent.push_back(entry);
        }

        cf_dir_next(&dir);
    }

    cf_dir_close(&dir);
}

std::string BLAengine::BlaFileTimeToString(BlaFileTime& blaFileTime)
{
    SYSTEMTIME utc;
    ::FileTimeToSystemTime(std::addressof(*(FILETIME*)&blaFileTime), std::addressof(utc));

    std::ostringstream stm;
    const auto w2 = std::setw(2);
    /*stm << std::setfill('0') << std::setw(4) << utc.wYear << '-' << w2 << utc.wMonth
        << '-' << w2 << utc.wDay << ' ' << w2 << utc.wYear << ' ' << w2 << utc.wHour
        << ':' << w2 << utc.wMinute << ':' << w2 << utc.wSecond << 'Z';*/

    stm << std::setfill('0') << std::setw(4) << utc.wYear << '-' << w2 << utc.wMonth
        << '-' << w2 << utc.wDay << ' ' << w2 << utc.wHour
        << ':' << w2 << utc.wMinute;

    return stm.str();
}

std::string BLAengine::BlaFileSizeToString(BlaFileSize& blaFileSize)
{
    if (blaFileSize > 10000)
    {
        return std::to_string(blaFileSize / 1000) + " Kb";
    }
    if (blaFileSize > 10e7)
    {
        return std::to_string(blaFileSize / 10e6) + " Mb";
    }
    if (blaFileSize > 10e10)
    {
        return std::to_string(blaFileSize / 10e9) + " Gb";
    }
    if (blaFileSize > 10e13)
    {
        return std::to_string(blaFileSize / 10e12) + " Tb";
    }
    
    return std::to_string(blaFileSize) + " bytes";
}

#include <External/Files/whereami.h>

std::string BLAengine::GetExecutablePath()
{
    int length = wai_getExecutablePath(NULL, 0, NULL);

    char* path = (char*)malloc(length + 1);

    wai_getExecutablePath(path, length, NULL);

    path[length] = 0;

    auto r = std::string(path);

    free(path);

    return r;
}

std::string BLAengine::GetModulePath()
{
    int length = wai_getModulePath(NULL, 0, NULL);

    char* path = (char*)malloc(length + 1);

    wai_getModulePath(path, length, NULL);

    path[length] = 0;

    auto r = std::string(path);

    free(path);

    return r;
}

std::string BLAengine::GetWorkingDir()
{
    //TODO : 
    return std::string("");
}
