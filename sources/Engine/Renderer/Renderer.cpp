#include "Renderer.h"

using namespace BLAengine;

void Renderer::SetCamera(Camera * camera)
{
	m_mainRenderCamera.AttachCamera(camera);
}

Renderer::Renderer()
{
	m_isContextEnabled = false;
	this->m_renderSize = ivec2(0,0);
}


Renderer::~Renderer(void)
{
}