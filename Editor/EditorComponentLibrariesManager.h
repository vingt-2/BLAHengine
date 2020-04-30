#pragma once
#include "Core/ComponentLibraries.h"

namespace BLA
{
	class EditorComponentLibrariesManager : public ComponentLibrariesManager
	{
	public:
		void LoadLibraries() override;
		void UnloadLibraries() override;

        void ReloadLibraries();
	};
}