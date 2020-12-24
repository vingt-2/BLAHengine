// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "EngineInstance.h"

using namespace BLA;

void RunScene()
{
    EngineInstance::AssignSingletonInstance(new EngineInstance(true, false));

    EngineInstance::LoopEngine();
}

int main()
{
    RunScene();
}

#ifdef WIN32

#include "Windows.h" 
int __stdcall WinMain(
    __in  HINSTANCE hInstance,
    __in  HINSTANCE hPrevInstance,
    __in  LPSTR lpCmdLine,
    __in  int nCmdShow)
{
    RunScene();
    return 0;
}

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif