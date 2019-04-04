#include "Renderer.h"

using namespace BLAengine;

void Renderer::SetCamera(CameraComponent * camera)
{
    m_mainRenderCamera.AttachCamera(camera);
}

Renderer::Renderer()
{
    m_isContextEnabled = false;
    this->m_renderSize = glm::vec2(0,0);
}