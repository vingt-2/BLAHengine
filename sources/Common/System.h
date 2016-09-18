#pragma once
#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INTERFACE
#ifdef GLFW_INTERFACE
	#define GLFW_INCLUDE_GLU
	#include <GLFW/glfw3.h>
#endif
