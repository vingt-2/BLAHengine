// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"

#include "System/Console.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "System/RenderWindow.h"
#include <random>
#include "Renderer/GPU/StaticBuffer.h"
#include "Renderer/RenderPass.h"

using namespace BLA;

const blaVector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

VulkanRenderer::VulkanRenderer(GLFWVulkanRenderWindow* pRenderWindow) : m_renderWindow(pRenderWindow)
{
    m_renderWindow->GetSize(m_viewPortExtents.x, m_viewPortExtents.y);

    CreateOrUpdateRenderTargets();
}

bool VulkanRenderer::Update()
{
    return true;
}

void VulkanRenderer::SetViewportSize(blaIVec2 renderSize)
{
    if(renderSize.x != m_viewPortExtents.x || renderSize.y != m_viewPortExtents.y)
    {
        m_viewPortExtents = renderSize;
        CreateOrUpdateRenderTargets();
    }
}

blaU32 VulkanRenderer::SetupAllRegisteredRenderPasses()
{
    RenderPassRegistry* registry = RenderPassRegistry::GetSingletonInstance();

    blaVector<blaU32> ids;
    registry->GetAllRenderPassIDs(ids);

    blaU32 c = 0;
    for(blaU32 id : ids)
    {
        c++;

        registry->GetRenderPassEntry(id)->m_pToInstanceRenderPassDescriptorPointer = new VulkanRenderPass();

    }
    return c;
}

void VulkanRenderer::CreateOrUpdateRenderTargets()
{
    if(m_offscreenBuffer.m_color)
    {
        m_offscreenBuffer.m_color->Cancel();
        delete m_offscreenBuffer.m_color;
    }

    Gpu::StaticBuffer<blaU32> buffer(m_viewPortExtents.x * m_viewPortExtents.y);

    {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> distrib(0xEF, 0xFF);

        memset(buffer.GetData(), distrib(gen) << 8 | distrib(gen) << 16 | distrib(gen) << 24 | 0xFF, buffer.GetLength() * buffer.GetElementSize());
    }
    m_offscreenBuffer.m_color = new Gpu::Image(blaIVec2(m_viewPortExtents.x, m_viewPortExtents.y), &buffer);

    // Submit for now is blocking, so we good ...
    m_offscreenBuffer.m_color->Submit();
}

RenderWindow* VulkanRenderer::GetRenderWindow()
{
    return static_cast<RenderWindow*>(m_renderWindow);
}
