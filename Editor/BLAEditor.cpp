// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "SceneEditor.h"

using namespace BLA;

void RunSceneEditor()
{
    EngineInstance::AssignSingletonInstance(new SceneEditor(true, false));

    EngineInstance::LoopEngine();
}

int main()
{
    RunSceneEditor();
}

#ifdef WIN32

#include "Windows.h" 
int __stdcall WinMain(
    __in  HINSTANCE hInstance,
    __in  HINSTANCE hPrevInstance,
    __in  LPSTR lpCmdLine,
    __in  int nCmdShow)
{
    RunSceneEditor();
    return 0;
}

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif