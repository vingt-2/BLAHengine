#include <thread>

#include "System/RenderWindow.h"
#include "EditorSession.h"
#include "System/Console.h"
#include "BLAStringID.h"

using namespace BLA;

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

#include "Windows.h" 
int __stdcall WinMain(
    __in  HINSTANCE hInstance,
    __in  HINSTANCE hPrevInstance,
    __in  LPSTR lpCmdLine,
    __in  int nCmdShow)
{
    RunEditorSession();
    return 0;
}

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif