#pragma once
#include "../../Std/Graphics.h"

class InputHandler
{
public:

	InputHandler(GLFWwindow* const windows);
	~InputHandler();
	
	bool reportControls(char keyPressed);

private:

	GLFWwindow* const m_glfwWindow;
	char m_controlMap[100];
};