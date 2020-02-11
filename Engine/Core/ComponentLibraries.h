#pragma once
#include "StdInclude.h"
#include "BLAStringID.h"

namespace BLAengine
{
    class GameComponentRegistry;
    class Console;
	class BLACORE_API ComponentLibrariesManager
	{
	public:
		virtual void LoadLibraries();
		virtual void UnloadLibraries();

	protected:
        static void SetLoadingLibrary(GameComponentRegistry* componentRegistry, Console* console, blaStringId libraryId);
        static void UnloadLibrary(GameComponentRegistry* componentRegistry, Console* console, blaStringId libraryId);

        blaVector<blaPair<blaStringId, void*>> m_loadedLibraries;
	};
}
