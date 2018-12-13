#include "Engine/Editor/EditorSession.h"
#include "HairDemo.h"
#include "AnimationDemo.h"

#ifdef BLA_NO_DLL
using namespace BLAengine;

Debug* g_debugInstance;

void DoEditorSession()
{
    EditorSession demo(true, false);

    GLFWRenderWindow* renderWindow = new GLFWRenderWindow();
    renderWindow->CreateRenderWindow("BLAengineDemo", 1280, 720, false); // Add arguments

    demo.InitializeEngine(renderWindow);

    demo.LoadWorkingScene("./Scenes/empty_scene");

    while (!demo.ShouldTerminate())
    {
        demo.UpdateEditor();
    }

    demo.TerminateEditor();

    demo.~EditorSession();
}

void DoHairDemoSession()
{
    HairDemo demo(true, false);

    GLFWRenderWindow* renderWindow = new GLFWRenderWindow();
    renderWindow->CreateRenderWindow("BLAengineDemo", 1280, 720, false); // Add arguments

    demo.InitializeEngine(renderWindow);

    g_debugInstance = demo.m_debug;

    demo.LoadWorkingScene("./Scenes/empty_scene");

    while (!demo.ShouldTerminate())
    {
        demo.UpdateEditor();
    }

    demo.TerminateEditor();

    demo.~HairDemo();
}

void DoAnimationDemoSession()
{
    AnimationDemo demo(true, false);

    GLFWRenderWindow* renderWindow = new GLFWRenderWindow();
    renderWindow->CreateRenderWindow("BLAengineDemo", 1280, 720, false); // Add arguments

    demo.InitializeEngine(renderWindow);

    g_debugInstance = demo.m_debug;

    demo.LoadWorkingScene("./Scenes/empty_scene");

    while (!demo.ShouldTerminate())
    {
        demo.UpdateEditor();
    }

    demo.TerminateEditor();

    demo.~AnimationDemo();
}

int main()
{
    //DoEditorSession();
    //DoHairDemoSession();
    DoAnimationDemoSession();
}

#endif