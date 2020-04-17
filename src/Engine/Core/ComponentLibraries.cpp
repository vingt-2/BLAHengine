#include <windows.h> // <-- Obviously needs to be abstracted away... Especially in Core. The editor version can stay windows specific
#include <Common/FileSystem/Files.h>
#include "ComponentLibraries.h"
#include "Engine/System/Console.h"

using namespace BLAengine;

void ComponentLibrariesManager::LoadLibraries()
{
	blaString libFolder = GetWorkingDir() + "Cooked/Libraries/"+ blaString(BLA_CONFIGURATION);

	blaVector<FileEntry> files;

	GetFilesInDirectory(files, libFolder);

	for(FileEntry& file : files)
	{
		if(file.m_extension == ".dll")
		{
			LoadLibraryA(file.GetFullPath().c_str());
		}
	}
}

void ComponentLibrariesManager::UnloadLibraries()
{
	//blaString libFolder = GetWorkingDir() + "Cooked\\Libraries\\";

	//blaVector<FileEntry> files;

	//GetFilesInDirectory(files, libFolder);

	//for (FileEntry& file : files)
	//{
	//	if (file.m_extension == ".dll")
	//	{
	//		LoadLibraryA(file.GetFullPath());
	//	}
	//}
}
