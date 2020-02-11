#include "Renderer.h"

using namespace BLAengine;

void Renderer::SetRenderSize(blaIVec2 renderSize)
{
    m_renderSize = renderSize;
}

void Renderer::SetCamera(CameraComponent * camera)
{
    m_mainRenderCamera.AttachCamera(camera);
}

Renderer::Renderer()
{
    m_renderToFrameBufferOnly = false;
    m_isContextEnabled = false;
    this->m_renderSize = glm::vec2(0, 0);
}