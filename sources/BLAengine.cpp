#include "Common/StdInclude.h"
#include "Engine/System/RenderWindow.h"
#include "AnimationDemo.h"
#include "./Common/DataStructures/Tree.h"

#include <thread>

#ifdef BLA_NO_DLL
using namespace BLAengine;

BLA_DECLARE_SINGLETON(EngineInstance)

void DoAnimationDemoSession()
{
    BLA_ASSIGN_SINGLETON(EngineInstance, new AnimationDemo(true, false));

    EngineInstance* demo;

    BLA_RETRIEVE_SINGLETON(EngineInstance, demo);

    auto renderWindow = new GLFWRenderWindow();

    renderWindow->CreateRenderWindow("BLAengine Animation Demo", 1280, 720, false);

    demo->InitializeEngine(renderWindow);

    demo->LoadWorkingScene("./Scenes/empty_scene");

	int framerate = 120;
	
	while (!demo->ShouldTerminate())
    {
		auto frameStarTime = std::chrono::steady_clock::now();
        demo->PreEngineUpdate();
        demo->EngineUpdate();
        demo->PostEngineUpdate();
		auto frameEndTime = std::chrono::steady_clock::now();
		int microSecondsFrameTime = (int) (1000000.f / (float)framerate);
		auto waitDuration = std::chrono::microseconds(microSecondsFrameTime) - (frameEndTime - frameStarTime);

		std::this_thread::sleep_for(waitDuration);
    }

    // Call terminates engine
    demo->TerminateEngine();
}

int main()
{
    DoAnimationDemoSession();
}

#endif