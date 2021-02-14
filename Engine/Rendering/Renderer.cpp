// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Renderer.h"

#include <random>
#include "Gpu/Interface.h"
#include "RenderPass.h"
#include "Gpu/RenderPassAttachment.h"

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(Renderer)
BLA_IMPLEMENT_SINGLETON(RenderPassRegistry)

void RenderTarget::OnChangeCalls()
{
    for(OnChange cb : m_callbacks)
    {
        cb(this);
    }
}

void RenderPassRegistry::__RegisterRenderPass(blaStringId stringId, blaU32 id, Gpu::RPAttachmentsDescription rpAttachmentDescription,
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& vertexAttributesDescriptors,
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptor,
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& opaqueValuesDescriptor)
{
    BLA_ASSERT(m_registry.find(id) == m_registry.end());

    m_registry.insert(std::make_pair(id, Gpu::RenderPassDescriptor{ stringId, rpAttachmentDescription, vertexAttributesDescriptors, uniformValuesDescriptor, opaqueValuesDescriptor}));
}

Gpu::RenderPassDescriptor* RenderPassRegistry::GetRenderPassEntry(blaU32 id)
{
    RenderPassRegistryStorage::iterator it = m_registry.find(id);
    if(it != m_registry.end())
    {
        return &it->second;
    }
    return nullptr;
}

const Gpu::RenderPassDescriptor* RenderPassRegistry::GetRenderPassEntry(blaU32 id) const
{
    RenderPassRegistryStorage::const_iterator it = m_registry.find(id);
    if (it != m_registry.end())
    {
        return &it->second;
    }
    return nullptr;
}

void RenderPassRegistry::GetAllRenderPassIDs(blaVector<blaU32>& stringIds) const
{
    for(auto e : m_registry)
    {
        stringIds.push_back(e.first);
    }
}

Renderer::Renderer(GLFWRenderWindow* pRenderWindow): m_renderWindow(pRenderWindow)
{
    m_renderWindow->GetSize(m_viewPortExtents.x, m_viewPortExtents.y);

    CreateOrUpdateRenderTargets();
}

bool Renderer::Update()
{
    RenderPassRegistry* registry = RenderPassRegistry::GetSingletonInstance();

    for(const auto& renderPassInstance : m_renderPassInstances)
    {
        renderPassInstance.second->Render();
    } 
    return true;
}

void Renderer::SetViewportSize(blaIVec2 renderSize)
{
    if (renderSize.x != m_viewPortExtents.x || renderSize.y != m_viewPortExtents.y)
    {
        m_viewPortExtents = renderSize;
        CreateOrUpdateRenderTargets();
    }
}

void Renderer::CreateOrUpdateRenderTargets()
{
    CreateOrUpdateOffscreenBuffer();
}

void Renderer::CreateOrUpdateOffscreenBuffer()
{
    if (m_offscreenBuffer.m_color)
    {
        m_offscreenBuffer.m_color->Cancel();
        m_offscreenBuffer.m_color = nullptr;
    }

    if (m_offscreenBuffer.m_depth)
    {
        m_offscreenBuffer.m_depth->Cancel();
        m_offscreenBuffer.m_depth = nullptr;
    }

    Gpu::StaticBuffer<blaU32> buffer(m_viewPortExtents.x * m_viewPortExtents.y, Gpu::BaseStaticBuffer::Usage::ImageBuffer);
    m_offscreenBuffer.m_color = new Gpu::Image<Gpu::Formats::R8G8B8A8_UNORM>(blaIVec2(m_viewPortExtents.x, m_viewPortExtents.y), buffer);

    Gpu::StaticBuffer<blaF32> depthBuffer(m_viewPortExtents.x * m_viewPortExtents.y, Gpu::BaseStaticBuffer::Usage::ImageBuffer);
    m_offscreenBuffer.m_depth = new Gpu::Image<Gpu::Formats::D32_SFLOAT>(blaIVec2(m_viewPortExtents.x, m_viewPortExtents.y), depthBuffer);

    // Submit for now is blocking, so we good ...
    m_offscreenBuffer.m_color->Submit();
    m_offscreenBuffer.m_depth->Submit();

    m_offscreenBuffer.OnChange();
}

void Renderer::CreateOrUpdateGeometryBuffer()
{
    if (m_geometryBuffer.m_diffuse)
    {
        m_geometryBuffer.m_diffuse->Cancel();
        m_geometryBuffer.m_diffuse = nullptr;
    }

    Gpu::StaticBuffer<blaU32> buffer(m_viewPortExtents.x * m_viewPortExtents.y, Gpu::BaseStaticBuffer::Usage::ImageBuffer);

    for (blaU32 i = 0; i < buffer.GetLength(); i++)
    {
        buffer[i] = 0;
    }

    m_geometryBuffer.m_diffuse = new Gpu::Image<Gpu::Formats::R8G8B8A8_UNORM>(blaIVec2(m_viewPortExtents.x, m_viewPortExtents.y), buffer);

    // Submit for now is blocking, so we good ...
    m_geometryBuffer.m_diffuse->Submit();
    m_geometryBuffer.m_diffuse->Submit();

    m_geometryBuffer.OnChange();
}

RenderWindow* Renderer::GetRenderWindow()
{
    return static_cast<RenderWindow*>(m_renderWindow);
}

