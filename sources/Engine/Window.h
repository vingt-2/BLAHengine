#pragma once
#include "Renderer.h"

class Window
{
public:

	Window(char* windowTitle,vec2 screenSize);

	~Window();
	
	GLFWwindow* InitializeContext(char* windowTitle,vec2 screenSize);
private:

	Renderer* renderer;

};