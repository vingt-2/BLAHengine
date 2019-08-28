#include <thread>

#include <Common/StdInclude.h>
#include <Engine/System/RenderWindow.h>
#include <Editor/EditorSession.h>
#include <Engine/System/Console.h>

#ifdef BLA_NO_DLL
using namespace BLAengine;

#pragma optimize("", off)

void RunEditorSession()
{
    EngineInstance::AssignSingletonInstance(new EditorSession(true, false));

    EngineInstance::LoopEngine();
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
    __in  int nCmdShow)
{
    RunEditorSession();
}
#endif

#endif