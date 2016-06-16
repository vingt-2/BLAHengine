#pragma once
#include "../../Common/Graphics.h"

class InputHandler
{
public:

	InputHandler(GLFWwindow* const windows);
	~InputHandler();
	
	bool isPressed(char keyPressed);

private:

	GLFWwindow* const m_glfwWindow;
	char m_controlMap[100];
};