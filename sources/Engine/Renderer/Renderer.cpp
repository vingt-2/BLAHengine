#include "Renderer.h"


void Renderer::SetCamera(Camera * camera)
{
	m_mainRenderCamera.AttachCamera(camera);
}

Renderer::Renderer(char* windowTitle,bool isFullScreen)
{
	m_isContextEnabled = false;
	this->m_renderSize = ivec2(1024,780);
	this->m_isFullScreen = isFullScreen;
}

Renderer::Renderer(char* windowTitle, bool isFullScreen, vec2 screenSize)
{
	m_isContextEnabled = false;
	this->m_renderSize = screenSize;
	this->m_isFullScreen = isFullScreen;
}


Renderer::~Renderer(void)
{
}