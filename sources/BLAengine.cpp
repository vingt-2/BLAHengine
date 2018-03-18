#include "Engine/Editor/EditorSession.h"
#ifdef BLA_NO_DLL
using namespace BLAengine;

int main()
{
    EditorSession demo(true, false);

    GLFWRenderWindow* renderWindow = new GLFWRenderWindow();
    renderWindow->CreateRenderWindow("BLAengineDemo", 512, 512, false); // Add arguments

    demo.InitializeEngine(renderWindow);

    demo.LoadWorkingScene("./Scenes/test_scene");

    while (!demo.ShouldTerminate())
    {
        demo.UpdateEditor();
    }

    demo.TerminateEditor();

    demo.~EditorSession();
}

#endif