// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Renderer.h"

#include <random>
#include "GPU/Interface.h"
#include "RenderPass.h"
#include "Gpu/RenderPassAttachment.h"
#include "Rendering/RenderPass.h"

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(RenderPassRegistry)

void RenderPassRegistry::__RegisterRenderPass(blaStringId stringId, blaU32 id, Gpu::RPAttachmentDescription rpAttachmentDescription,
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& vertexAttributesDescriptors,
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptor)
{
    BLA_ASSERT(m_registry.find(id) == m_registry.end());

    m_registry.insert(std::make_pair(id, Gpu::RenderPassDescriptor{ stringId, rpAttachmentDescription, nullptr, vertexAttributesDescriptors, uniformValuesDescriptor }));
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
    blaVector<blaU32> rpIds;
    RenderPassRegistry::GetSingletonInstanceRead()->GetAllRenderPassIDs(rpIds);

    m_renderWindow->GetSize(m_viewPortExtents.x, m_viewPortExtents.y);

    CreateOrUpdateRenderTargets();
    
    // Setup Test Render Pass...
    SetupRenderPassObjects();
}

bool Renderer::Update()
{
    RenderPassRegistry* registry = RenderPassRegistry::GetSingletonInstance();

    if (Gpu::RenderPassDescriptor* geometryPassDesc = registry->GetRenderPassEntry(BlaStringId("TestMeshPass")))
    {
        Gpu::Interface* gpu = Gpu::Interface::GetSingletonInstance();
        gpu->Render(*geometryPassDesc);
    }
    return true;
}

void Renderer::SetViewportSize(blaIVec2 renderSize)
{
    if (renderSize.x != m_viewPortExtents.x || renderSize.y != m_viewPortExtents.y)
    {
        m_viewPortExtents = renderSize;
        CreateOrUpdateRenderTargets();

        RenderPassRegistry* registry = RenderPassRegistry::GetSingletonInstance();

        if (Gpu::RenderPassDescriptor* geometryPassDesc = registry->GetRenderPassEntry(BlaStringId("TestMeshPass")))
        {
            Gpu::AttachmentDesc<Gpu::Formats::R8G8B8A8_UNORM> colorAttachment(*m_offscreenBuffer.m_color);

            DeclareRenderPassAttachment(OffscreenRenderTargetAttachment, ColorAttachments(Gpu::Formats::R8G8B8A8_UNORM), Gpu::Formats::D32_SFLOAT);

            OffscreenRenderTargetAttachment::ColorAttachments colorAttachments(colorAttachment);
            Gpu::AttachmentDesc<Gpu::Formats::D32_SFLOAT> depthAttachment(*m_offscreenBuffer.m_depth);

            OffscreenRenderTargetAttachment attachment(colorAttachments, depthAttachment);

            // Gpu::RenderPassAttachment attachment(m_offscreenBuffer.m_color);
            //Gpu::Interface::GetSingletonInstance()->AttachToRenderPass(*geometryPassDesc, attachment);
        }
    }
}

#include "Gpu/RenderPassProgram.h"
#include "Gpu/Vulkan.h"
void Renderer::SetupRenderPassObjects()
{
    RenderPassRegistry* registry = RenderPassRegistry::GetSingletonInstance();

    if (Gpu::RenderPassDescriptor* geometryPassDesc = registry->GetRenderPassEntry(BlaStringId("TestMeshPass")))
    {
        Gpu::ShaderProgram vertexShader(Gpu::ShaderProgram::Type::VertexShader, "./resources/shaders/Vulkan/Engine/TestMeshPassVert.spv");
        Gpu::ShaderProgram fragmentShader(Gpu::ShaderProgram::Type::FragmentShader, "./resources/shaders/Vulkan/Engine/TestMeshPassFrag.spv");

        vertexShader.Submit();
        fragmentShader.Submit();

        Gpu::RenderPassProgram program;
        program.m_shaders.push_back(vertexShader);
        program.m_shaders.push_back(fragmentShader);

        // Gpu::RenderPassAttachment< attachment(m_offscreenBuffer.m_color);

        Gpu::Interface::GetSingletonInstance()->SetupRenderPass(*geometryPassDesc, program);
        // Gpu::Interface::GetSingletonInstance()->AttachToRenderPass(*geometryPassDesc, attachment);
    }
}

blaU32 Renderer::SetupAllRegisteredRenderPasses()
{
    RenderPassRegistry* registry = RenderPassRegistry::GetSingletonInstance();

    blaVector<blaU32> ids;
    registry->GetAllRenderPassIDs(ids);

    Gpu::Interface* gpu = Gpu::Interface::GetSingletonInstance();

    blaU32 c = 0;
    for (blaU32 id : ids)
    {
        c++;

        // registry->GetRenderPassEntry(id)->m_pToInstanceRenderPassDescriptorPointer = gpu->SetupRenderPass(*registry->GetRenderPassEntry(id),);
    }
    return c;
}

void Renderer::CreateOrUpdateRenderTargets()
{
    if (m_offscreenBuffer.m_color)
    {
        m_offscreenBuffer.m_color->Cancel();
        m_offscreenBuffer.m_color = nullptr;
    }

    Gpu::StaticBuffer<blaU32> buffer(m_viewPortExtents.x * m_viewPortExtents.y, Gpu::BaseStaticBuffer::Usage::ImageBuffer);

    for(blaU32 i = 0; i < buffer.GetLength(); i++)
    {
        buffer[i] = 0;
    }
   
    m_offscreenBuffer.m_color = new Gpu::Image<Gpu::Formats::R8G8B8A8_UNORM>(blaIVec2(m_viewPortExtents.x, m_viewPortExtents.y), buffer);

    Gpu::StaticBuffer<blaF32> depthBuffer(m_viewPortExtents.x * m_viewPortExtents.y, Gpu::BaseStaticBuffer::Usage::ImageBuffer);

    for (blaU32 i = 0; i < depthBuffer.GetLength(); i++)
    {
        depthBuffer[i] = 0;
    }

    m_offscreenBuffer.m_depth = new Gpu::Image<Gpu::Formats::D32_SFLOAT>(blaIVec2(m_viewPortExtents.x, m_viewPortExtents.y), depthBuffer);

    // Submit for now is blocking, so we good ...
    m_offscreenBuffer.m_color->Submit();
    m_offscreenBuffer.m_depth->Submit();
}

RenderWindow* Renderer::GetRenderWindow()
{
    return static_cast<RenderWindow*>(m_renderWindow);
}

