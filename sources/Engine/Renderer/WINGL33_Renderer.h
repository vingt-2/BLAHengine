#pragma once
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