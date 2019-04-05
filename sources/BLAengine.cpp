#include <thread>

#include <Common/StdInclude.h>
#include <Engine/System/RenderWindow.h>
#include <Editor/EditorSession.h>
#include <Common/DataStructures/StringID.h>

#ifdef BLA_NO_DLL
using namespace BLAengine;

void RunEditorSession()
{
    EngineInstance::AssignSingletonInstance(new EditorSession(true, false));

    EngineInstance* editorSession = EngineInstance::GetSingletonInstance();

    RenderWindow* renderWindow = new GLFWRenderWindow();

    renderWindow->CreateRenderWindow("BLAengine Editor", 1280, 720, false);

    editorSession->InitializeEngine(renderWindow);

    int framerate = 144;

    while (!editorSession->ShouldTerminate())
    {
        auto frameStartTime = std::chrono::steady_clock::now();

        editorSession->PreEngineUpdate();
        editorSession->EngineUpdate();
        editorSession->PostEngineUpdate();

        auto frameEndTime = std::chrono::steady_clock::now();
        int microSecondsFrameTime = (int) (1000000.f / (float)framerate);
        auto waitDuration = std::chrono::microseconds(microSecondsFrameTime) - (frameEndTime - frameStartTime);

        std::this_thread::sleep_for(waitDuration);
    }

    // Call terminates engine
    editorSession->TerminateEngine();
}

int main()
{
    RunEditorSession();
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
    blaU32 stringId = COMPILE_TIME_CRC32_STR("helloworld");

    RunEditorSession();
}
#endif

#endif