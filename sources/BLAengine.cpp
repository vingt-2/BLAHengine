#include <thread>

#include <Common/StdInclude.h>
#include <Engine/System/RenderWindow.h>
#include <Demos/AnimationDemo.h>
#include <Common/DataStructures/Tree.h>
#include <Engine/Gui/Gui.h>


#ifdef BLA_NO_DLL
using namespace BLAengine;

void DoAnimationDemoSession()
{
    EngineInstance::AssignSingletonInstance(new AnimationDemo(true, false));

    EngineInstance* demo = EngineInstance::GetSingletonInstance();

    auto renderWindow = new GLFWRenderWindow();

    renderWindow->CreateRenderWindow("BLAengine Animation Demo", 1280, 720, false);

    demo->InitializeEngine(renderWindow);

    demo->LoadWorkingScene("./Scenes/floor_scene");

    int framerate = 50;

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

#endif