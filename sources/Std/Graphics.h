//#define USE_OSXGL32_RENDERER
#define USE_WINGL33_RENDERER

#ifdef USE_OSXGL32_RENDERER
#pragma once
// Include GLFW
#define GLFW_INCLUDE_GLU
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#endif

#ifdef USE_WINGL33_RENDERER
#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
// Include GLFW
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#endif

