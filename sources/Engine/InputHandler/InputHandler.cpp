#include "InputHandler.h"

InputHandler::InputHandler(GLFWwindow* const window): 
	m_glfwWindow(window)
{}

InputHandler::~InputHandler(){};

bool InputHandler::isPressed(char keyPressed)
{
	if ((glfwGetKey(m_glfwWindow, keyPressed) == GLFW_PRESS))
	{
		return true;
	}

	return false;
}
