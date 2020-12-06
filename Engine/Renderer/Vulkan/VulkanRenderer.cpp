// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"

#include "System/Console.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "System/RenderWindow.h"
#include <random>

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

void VulkanRenderer::CreateOrUpdateRenderTargets()
{
    if(m_offscreenBuffer.m_color.Get())
    {
        m_offscreenBuffer.m_color.Cancel();
        Gpu::Image::Delete(m_offscreenBuffer.m_color.Get());
    }

    Gpu::Image * image = Gpu::Image::New(blaIVec2(m_viewPortExtents.x, m_viewPortExtents.y), sizeof(blaU32));

    m_offscreenBuffer.m_color = Gpu::Resource<Gpu::Image>(*image);

    // Submit for now is blocking, so we good ...
    m_offscreenBuffer.m_color.Submit();

    //std::random_device rd;  //Will be used to obtain a seed for the random number engine
    //std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    //std::uniform_int_distribution<> distrib(0xFF, 0xEF);

    //blaVector<blaU32> dummydata(m_viewPortExtents.x * m_viewPortExtents.y, distrib(gen) << 8 | distrib(gen) << 16 | distrib(gen) << 24| 0xFF);

    // VkBuffer stagingBuffer;
    // VkDeviceMemory stagingBufferMemory;

    //  m_renderWindow->GetVulkanInterface()->CreateBuffer(
    //    sizeof(blaU32) * dummydata.size(),
    //    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
    //    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
    //    stagingBuffer, stagingBufferMemory);

    //void* data;
    //vkMapMemory(device, stagingBufferMemory, 0, sizeof(blaU32) * dummydata.size(), 0, &data);
    //memcpy(data, dummydata.data(), static_cast<size_t>(sizeof(blaU32) * dummydata.size()));
    //vkUnmapMemory(device, stagingBufferMemory);

    // m_renderWindow->GetVulkanInterface()->CopyBufferToImage(stagingBuffer, m_offscreenBuffer.m_color.m_image, m_viewPortExtents.x, m_viewPortExtents.y);
}

RenderWindow* VulkanRenderer::GetRenderWindow()
{
    return static_cast<RenderWindow*>(m_renderWindow);
}
