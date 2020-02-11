#pragma once
#include "Core/ComponentLibraries.h"

namespace BLAengine
{
	class EditorComponentLibrariesManager : public ComponentLibrariesManager
	{
	public:
		void LoadLibraries() override;
		void UnloadLibraries() override;

        void ReloadLibraries();
	};
}