#include "Renderer.h"


Renderer::Renderer(char* windowTitle,bool isFullScreen)
{
	isContextEnabled = false;
	this->renderSize = vec2(1024,780);
	this->isFullScreen = isFullScreen;
	this->m_directionalLight = vec3(1, 0, 0);
}

Renderer::Renderer(char* windowTitle, bool isFullScreen, vec2 screenSize)
{
	isContextEnabled = false;
	this->renderSize = screenSize;
	this->isFullScreen = isFullScreen;
}


Renderer::~Renderer(void)
{
}

void Renderer::Resize(int xRes,int yRes)
{
	 glfwSetWindowSize(glfwWindow,xRes,yRes);
	 renderSize.x = xRes;
	 renderSize.y = yRes;
}
