#include "EngineDemo.h"
#ifdef BLA_NO_DLL
using namespace BLAengine;

int main()
{
	Raytracer demo(true, false);

	demo.InitializeEngine();

	while (!demo.ShouldTerminate())
	{
		demo.UpdateEditor();
	}

	demo.TerminateEditor();

	demo.~Raytracer();
}

#endif