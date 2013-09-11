/*
    GL33 Renderer implements the windows OpenGL3.3 Renderer used on Windows platform and uses GLEW to load modern GL.
*/
#include "Renderer.h"

class GL33Renderer : public Renderer
{
public:
    bool Update();
    
    GL33Renderer(char* windowTitle,bool isFullScreen);
    GL33Renderer(char* windowTitle,bool isFullScreen, vec2 renderSize);
    ~GL33Renderer();
    
protected:
    GLFWwindow* InitializeContext(char* windowTitle);
};