#include <windows.h> // <-- Obviously needs to be abstracted away... Especially in Core. The editor version can stay windows specific
#include "System/FileSystem/Files.h"
#include "System/Console.h"
#include "Core/GameComponent.h"
#include "ComponentLibraries.h"

//TODO: Cmake handles that ...
#define BLA_CONFIGURATION "Release"

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
}

void ComponentLibrariesManager::SetLoadingLibrary(GameComponentRegistry* componentRegistry, Console* console, blaStringId libraryId)
{
    componentRegistry->m_currentRegisteringLibrary = libraryId;

    console->m_currentRegisteringLibrary = libraryId;
}

void ComponentLibrariesManager::UnloadLibrary(GameComponentRegistry* componentRegistry, Console* console, blaStringId libraryId)
{
    componentRegistry->UnloadLibraryComponent(libraryId);
    console->UnloadConsoleCommandsForLibrary(libraryId);
}
