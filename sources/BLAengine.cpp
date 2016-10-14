#include "EngineDemo.h"
#ifdef BLA_NO_DLL
using namespace BLAengine;

int main()
{
	EngineDemo demo(true, false);

	GLFWRenderWindow* renderWindow = new GLFWRenderWindow();
	renderWindow->CreateRenderWindow("BLAengineDemo", 100, 100, false); // Add arguments

	demo.InitializeDemo(renderWindow);

	while (!demo.ShouldTerminate())
	{
		demo.UpdateDemo();
	}

	demo.TerminateDemo();

	demo.~EngineDemo();
}

#endif