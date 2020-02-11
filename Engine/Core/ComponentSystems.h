#pragma once
#include "StdInclude.h"

namespace BLAengine
{
	class ComponentSystem
	{
	public:
		virtual void LoadLibraries();
		virtual void UnloadLibraries();
	};
}