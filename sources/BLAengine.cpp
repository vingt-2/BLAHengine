#include <thread>

#include <Common/StdInclude.h>
#include <Engine/System/RenderWindow.h>
#include <Demos/AnimationDemo.h>

#ifdef BLA_NO_DLL
using namespace BLAengine;

void DoAnimationDemoSession()
{
    EngineInstance::AssignSingletonInstance(new AnimationDemo(true, false));

    EngineInstance* demo = EngineInstance::GetSingletonInstance();

    auto renderWindow = new GLFWRenderWindow();

    renderWindow->CreateRenderWindow("BLAengine Animation Demo", 1280, 720, false);

    demo->InitializeEngine(renderWindow);

    demo->LoadWorkingScene("./Scenes/empty_scene");

    int framerate = 1050;

    while (!demo->ShouldTerminate())
    {
        auto frameStartTime = std::chrono::steady_clock::now();

        demo->PreEngineUpdate();
        demo->EngineUpdate();
        demo->PostEngineUpdate();

        auto frameEndTime = std::chrono::steady_clock::now();
        int microSecondsFrameTime = (int) (1000000.f / (float)framerate);
        auto waitDuration = std::chrono::microseconds(microSecondsFrameTime) - (frameEndTime - frameStartTime);

        std::this_thread::sleep_for(waitDuration);
    }

    // Call terminates engine
    demo->TerminateEngine();
}

int main()
{
    DoAnimationDemoSession();
}

#ifdef WIN32

#include "windows.h"
int CALLBACK WinMain(
    __in  HINSTANCE hInstance,
    __in  HINSTANCE hPrevInstance,
    __in  LPSTR lpCmdLine,
    __in  int nCmdShow
    )
{
    DoAnimationDemoSession();
}
#endif

#endif