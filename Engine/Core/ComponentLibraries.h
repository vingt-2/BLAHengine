#pragma once
#include "StdInclude.h"
#include "BLAStringID.h"

namespace BLA
{
    class GameComponentRegistry;
	class ComponentSystemsRegistry;
    class Console;
	class BLACORE_API ComponentLibrariesManager
	{
	public:
		virtual void LoadLibraries();
		virtual void UnloadLibraries();

	protected:
        static void SetLoadingLibrary(GameComponentRegistry* componentRegistry, ComponentSystemsRegistry* systemsRegistry, Console* console, blaStringId libraryId);
        static void UnloadLibrary(GameComponentRegistry* componentRegistry, ComponentSystemsRegistry* systemsRegistry, Console* console, blaStringId libraryId);

        blaVector<blaPair<blaStringId, void*>> m_loadedLibraries;
	};
}
