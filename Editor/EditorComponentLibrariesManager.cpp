// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "windows.h"
#include "System/FileSystem/Files.h"
#include "System/Console.h"
#include "Core/ComponentSystems.h"
#include "Core/Scene.h"

#include "EditorComponentLibrariesManager.h"

//TODO: Make this cross-platformer
#include "fileapi.h"
//TODO: Cmake should handle that ...
#define BLA_CONFIGURATION "RelWithDebInfo"

using namespace BLA;

void EditorComponentLibrariesManager::CreateTemporaryLibaryStorage()
{
	// CreateDirectoryA("./Temp",LPSECURITY_ATTRIBUTES)
}

void EditorComponentLibrariesManager::DeleteTemporaryLibraryStorage()
{
}

void EditorComponentLibrariesManager::LoadLibraries()
{
	Console::LogMessage("Editor Loading Libraries:");

	blaString libFolder = GetWorkingDir() + "Cooked/Libraries/"+ blaString(BLA_CONFIGURATION);

	blaVector<FileEntry> files;

	GetFilesInDirectory(files, libFolder);

	for(FileEntry& file : files)
	{
        GameComponentRegistry* componentRegistry = GameComponentRegistry::GetSingletonInstance();
        ComponentSystemsRegistry* systemsRegistry = ComponentSystemsRegistry::GetSingletonInstance();
        Console* console = Console::GetSingletonInstance();

        blaStringId libraryId = GenerateBlaStringId(file.m_name);

        // We need to inform the component registry and the console that the component declaration and console command / var declarations belong to this specific library
        SetLoadingLibrary(componentRegistry, systemsRegistry, console, libraryId);

		if(file.m_extension == ".dll")
		{
            if (file.m_name[0] == '_') 
            {
                continue;
            }
			HMODULE load = LoadLibraryA(file.GetFullPath().c_str());
			Console::LogMessage("\tLoading: " + file.m_name + file.m_extension + "| Status: " + (load != NULL ? "Ok" : "Fail"));

            if(!load) // We might have loaded components, we must presume that they are invalid, need to unload everything
            {
                UnloadLibrary(componentRegistry, systemsRegistry, console, libraryId);
            }
            else 
            {
                m_loadedLibraries.push_back(blaPair<blaStringId, void*>(libraryId, load));
            }
		}
        systemsRegistry->FinalizeLoad();
	}
}

void EditorComponentLibrariesManager::UnloadLibraries()
{
    GameComponentRegistry* componentRegistry = GameComponentRegistry::GetSingletonInstance();
    ComponentSystemsRegistry* systemsRegistry = ComponentSystemsRegistry::GetSingletonInstance();
    Console* console = Console::GetSingletonInstance();

    for (auto library : m_loadedLibraries)
    {
        UnloadLibrary(componentRegistry, systemsRegistry, console, library.first);
        FreeLibrary((HMODULE)library.second);
    }
}

void EditorComponentLibrariesManager::ReloadLibraries()
{
    UnloadLibraries();
    LoadLibraries();
}