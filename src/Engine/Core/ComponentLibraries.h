#pragma once
#include "Common/StdInclude.h"

namespace BLAengine
{
	class ComponentLibrariesManager
	{
	public:
		virtual void LoadLibraries();
		virtual void UnloadLibraries();
	};
}