/*
 OSX GL32 Renderer implements the OSX GL 3.2 renderer
 */

#pragma once
#include "Renderer.h"

class OSXGL32Renderer : public Renderer
{
public:
    bool Update();
    
    OSXGL32Renderer(char* windowTitle,bool isFullScreen);
    OSXGL32Renderer(char* windowTitle,bool isFullScreen,vec2 renderSize);
    ~OSXGL32Renderer();
    
    
    
protected:
    GLFWwindow* InitializeContext(char* windowTitle);
};
