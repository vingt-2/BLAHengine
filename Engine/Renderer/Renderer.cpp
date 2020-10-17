// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Renderer.h"
#include "RenderPass.h"

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(RenderPassRegistry)

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

void RenderPassRegistry::__RegisterRenderPass(blaU32 id, blaU32 attachmentCount,
    blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& vertexAttributesDescriptors,
    blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptor)
{
    BLA_ASSERT(m_registry.find(id) == m_registry.end());

    m_registry.insert(std::make_pair(id, RenderPassRegistryEntry{ attachmentCount, vertexAttributesDescriptors, uniformValuesDescriptor }));
}
