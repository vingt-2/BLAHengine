// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Renderer.h"

using namespace BLA;

void Renderer::SetRenderSize(blaIVec2 renderSize)
{
    m_renderSize = renderSize;
}

void Renderer::SetCamera(CameraComponent * camera)
{
    m_mainRenderCamera.AttachCamera(camera);
}

Renderer::Renderer(const AssetManager* assetManager)
{
    m_assetManager = assetManager;
    m_renderToFrameBufferOnly = false;
    m_isContextEnabled = false;
    this->m_renderSize = glm::vec2(0, 0);
}