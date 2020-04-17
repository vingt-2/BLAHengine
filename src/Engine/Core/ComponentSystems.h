#pragma once
#include "Common/StdInclude.h"

namespace BLAengine
{
	class ComponentSystem
	{
	public:
		virtual void LoadLibraries();
		virtual void UnloadLibraries();
	};
}