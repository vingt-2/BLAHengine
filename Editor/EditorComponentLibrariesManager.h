#pragma once
#include "Core/ComponentLibraries.h"

namespace BLA
{
	class EditorComponentLibrariesManager : public ComponentLibrariesManager
	{
		void CreateTemporaryLibaryStorage();
		void DeleteTemporaryLibraryStorage();
	public:
		void LoadLibraries() override;
		void UnloadLibraries() override;

        void ReloadLibraries();
	};
}