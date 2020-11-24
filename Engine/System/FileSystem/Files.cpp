// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Files.h"

#include "External/Files/cute_files.h"

#include <iomanip>
#include <sstream>
#include "External/Files/dirent.h"

using namespace BLA;

void BLA::GetAllContentInDirectory(blaVector<DirectoryEntry>& directoryContent, const blaString& inDirectoryPath)
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
            if (posExt != blaString::npos)
            {
                entry.m_extension = entry.m_name.substr(posExt);
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

void BLA::GetDirectoriesInDirectory(blaVector<DirectoryEntry>& directoryContent, const blaString& inDirectoryPath)
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

void BLA::GetFilesInDirectory(blaVector<FileEntry>& directoryContent, const blaString& inDirectoryPath)
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
			entry = ParseFilePath(file.path);
            cf_get_file_time(file.path, (cf_time_t*)&(entry.m_lastEditTime));
            directoryContent.push_back(entry);
        }

        cf_dir_next(&dir);
    }

    cf_dir_close(&dir);
}

blaString BLA::BlaFileTimeToString(BlaFileTime& blaFileTime)
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

blaString BLA::BlaFileSizeToString(BlaFileSize& blaFileSize)
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

#include "External/Files/whereami.h"

blaString BLA::GetExecutablePath()
{
    int length = wai_getExecutablePath(NULL, 0, NULL);

    char* path = (char*)malloc(length + 1);

    wai_getExecutablePath(path, length, NULL);

    path[length] = 0;

    auto r = blaString(path);

    free(path);

    return r;
}

blaString BLA::GetModulePath()
{
    int length = wai_getModulePath(NULL, 0, NULL);

    char* path = (char*)malloc(length + 1);

    wai_getModulePath(path, length, NULL);

    path[length] = 0;

    auto r = blaString(path);

    free(path);

    return r;
}

blaString BLA::GetWorkingDir()
{
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	blaString s = buf;
	std::replace(s.begin(), s.end(), '\\', '/');
	return s + "/";
}

FileEntry BLA::ParseFilePath(const blaString &filepath)
{
    FileEntry file;

    blaSize i = filepath.find_last_of('/');
    blaSize j = filepath.find_last_of('\\');

    i = i == blaString::npos ? 0 : i;
    j = j == blaString::npos ? 0 : j;

    const blaSize lastSlash = (i > j ? i : j) + 1;

    blaSize lastDot = filepath.find_last_of('.');

    if (lastDot == blaString::npos)
    {
        lastDot = filepath.length();
        file.m_extension = "";
    }
    else
    {
        file.m_extension = filepath.substr(lastDot, filepath.length() - lastDot);
    }

    file.m_path = filepath.substr(0, lastSlash);
    file.m_name = filepath.substr(lastSlash, lastDot - lastSlash);

    file.m_lastEditTime = 0;
    file.m_size = 0;

    return file;
}
