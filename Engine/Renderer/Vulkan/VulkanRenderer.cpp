// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"
#include "Renderer/RenderPass.h"

#if NEW_VULKAN_RENDERER

#define GLFW_INCLUDE_VULKAN

#include "RenderBackend.h"
#include "System/Console.h"
#include "System/RenderWindow.h"
#include "Renderer/MeshRendererComponent.h"
#include "Renderer/PointLightComponent.h"
#include "core/InspectableVariables.h"

#include "System/VulkanRenderWindow.h"
#include <optional>
#include "VulkanRenderer.h"
#include <set>

#define VK_CHECK_RESULT(x) x

using namespace BLA;

// debug test test test

#include "Renderer/RenderPass.h"

DeclareRenderPass(TestRenderPass, VertexAttributes(blaVec3, blaVec2), UniformValues(blaVec3, blaF32), 1)

const blaVector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

struct BLA::TextureView
{
    VkImage m_image;
    VkImageView m_imageView;
    VkDeviceMemory m_memory;
};

std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VulkanRenderer::VulkanRenderer(const AssetManager* assetManager)
{}

TestRenderPass::RenderPassInstance* g_instance;
blaVector<blaVec3> g_data;
blaVector<blaVec2> g_data2_dummy;
blaVec3 g_colorUniformAttribute;

void SetupDebugRenderPass()
{
    g_data = {
        blaVec3(-1.0f, -1.0f, 0.0f),
        blaVec3(1.0f, -1.0f, 0.0f),
        blaVec3(-1.0f, 1.0f, 0.0f),
        blaVec3(-1.0f, 1.0f, 0.0f),
        blaVec3(1.0f, -1.0f, 0.0f),
        blaVec3(1.0f, 1.0f, 0.0f),
    };

    g_data2_dummy = { blaVec2(0,0) };

    g_colorUniformAttribute = blaVec3(1, 0, 0);

    const TestRenderPass::RenderPassInstance::InstanceVertexAttributes quadVA(g_data,g_data2_dummy);
    const TestRenderPass::RenderPassInstance::InstanceUniformValues colorUniform(g_colorUniformAttribute, 1);

    g_instance = new TestRenderPass::RenderPassInstance(quadVA, colorUniform);

    blaString a = BLA::GenerateVulkanShaderTemplate<TestRenderPass>();

    g_instance->GetVertexAttributes<0>();
    g_instance->GetVertexAttributes<1>();
}


template<int i, class RenderPass>
class GetRenderPassVADescriptorsInternal
{
public:
    static void Get(blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
    {
        GetRenderPassVADescriptorsInternal<i - 1, RenderPass>::Get(typeDescriptors);
        BLAInspectableVariables::ExposedVarTypeDescriptor* d = 
            BLAInspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<i, typename RenderPass::RenderPassInstance::InstanceVertexAttributes>::VAType>::GetDescriptor();

        typeDescriptors.push_back(d);
    }
};

template<class RenderPass>
class GetRenderPassVADescriptorsInternal<0, RenderPass>
{
public:
    static void Get(blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
    {
        BLAInspectableVariables::ExposedVarTypeDescriptor* d = 
            BLAInspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<0, typename RenderPass::RenderPassInstance::InstanceVertexAttributes>::VAType>::GetDescriptor();

        typeDescriptors.push_back(d);
    }
};

template<class RenderPass>
blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassVADescriptors()
{
    blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

    GetRenderPassVADescriptorsInternal<RenderPass::VACount - 1, RenderPass>::Get(typeDescriptors);

    return typeDescriptors;
}

template<class RenderPass>
blaString BLA::GenerateVulkanShaderTemplate()
{
    blaString r = "";
    for(auto d : GetRenderPassVADescriptors<RenderPass>())
    {
        r += blaString(d->m_typeID) + ", ";
    }

    return r;
}

void VulkanRenderer::InitializeRenderer(RenderWindow* renderWindow)
{
    m_renderWindow = dynamic_cast<GLFWVulkanRenderWindow*>(renderWindow);

    m_renderWindow->GetSize(m_viewPortExtents.x, m_viewPortExtents.y);

    SetupDebugRenderPass();

    CreateDisplayBuffers();
}

bool VulkanRenderer::Update()
{	
	return true;
}

void VulkanRenderer::CleanupRenderer()
{
}

void VulkanRenderer::CreateDisplayBuffers()
{
    VkImageCreateInfo imageCreate = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    memset(&imageCreate, 0, sizeof(imageCreate));

    imageCreate.extent.width = m_viewPortExtents.x;
    imageCreate.extent.height = m_viewPortExtents.y;
    imageCreate.extent.depth = 1;
    imageCreate.imageType = VK_IMAGE_TYPE_2D;
    imageCreate.format = VK_FORMAT_R8G8B8_UNORM;
    imageCreate.mipLevels = 1;
    imageCreate.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreate.tiling = VK_IMAGE_TILING_OPTIMAL;
    // We will sample directly from the color attachment
    imageCreate.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    VkMemoryAllocateInfo memAlloc{};
    VkMemoryRequirements memReqs;

    VkDevice device = m_renderWindow->GetVulkanContext()->m_Device;

    m_offscreenBuffer.m_color = new TextureView();

    VK_CHECK_RESULT(vkCreateImage(device, &imageCreate, nullptr, &m_offscreenBuffer.m_color->m_image));
    vkGetImageMemoryRequirements(device, m_offscreenBuffer.m_color->m_image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = 0;// d->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &m_offscreenBuffer.m_color->m_memory));
    VK_CHECK_RESULT(vkBindImageMemory(device, m_offscreenBuffer.m_color->m_image, m_offscreenBuffer.m_color->m_memory, 0));

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_offscreenBuffer.m_color->m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &m_offscreenBuffer.m_color->m_imageView));
}

#endif
