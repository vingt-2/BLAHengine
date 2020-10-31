// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"

#if NEW_VULKAN_RENDERER

#define GLFW_INCLUDE_VULKAN

#include "RenderBackend.h"
#include "System/Console.h"
#include "System/RenderWindow.h"
#include "Renderer/MeshRendererComponent.h"
#include "Renderer/PointLightComponent.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "System/VulkanRenderWindow.h"
#include <optional>
#include "VulkanRenderer.h"
#include <set>

#define VK_CHECK_RESULT(x) x

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(VulkanRenderer)

DeclareRenderPass(TestRenderPass, VertexAttributes(blaVec3, blaVec2), UniformValues(blaVec3, blaF32), 1)

RegisterRenderPass(TestRenderPass)

const blaVector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

struct BLA::TextureView
{
    VkImage m_image;
    VkImageView m_imageView;
    VkDeviceMemory m_memory;
};

RenderPassManager::RenderPassManager(const blaVector<blaU32>& rpIds)
{
    m_renderPasses = rpIds;
}

VulkanRenderer::VulkanRenderer(const AssetManager* assetManager, const blaVector<blaU32>& rpIds) : m_renderPassManager(rpIds)
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

    Console::LogMessage("Here's what's in the TestRenderPass");
    Console::LogMessage(BLA::GenerateVulkanShaderTemplate<TestRenderPass>());
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

    // CreateDisplayBuffers();

    //Todo: This should take into account project hotloading
    blaVector<blaU32> rps;
    RenderPassRegistry::GetSingletonInstanceRead()->GetAllRenderPassIDs(rps);
}

RenderPassManager* VulkanRenderer::GetRenderPassManager()
{
    return &m_renderPassManager;
}

template<typename RenderPass>
void SetupRenderPass(RenderPass rp)
{
	// Set up separate renderpass with references to the color and depth attachments
	std::array<VkAttachmentDescription, 4> attachmentDescs = {};

	// Init attachment properties
	for (uint32_t i = 0; i < 4; ++i)
	{
		attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		if (i == 3)
		{
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		else
		{
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}

	// Formats
	attachmentDescs[0].format = offScreenFrameBuf.position.format;
	attachmentDescs[1].format = offScreenFrameBuf.normal.format;
	attachmentDescs[2].format = offScreenFrameBuf.albedo.format;
	attachmentDescs[3].format = offScreenFrameBuf.depth.format;

	std::vector<VkAttachmentReference> colorReferences;
	colorReferences.push_back({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	colorReferences.push_back({ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	colorReferences.push_back({ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 3;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pColorAttachments = colorReferences.data();
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
	subpass.pDepthStencilAttachment = &depthReference;

	// Use subpass dependencies for attachment layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pAttachments = attachmentDescs.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 2;
	renderPassInfo.pDependencies = dependencies.data();

	VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &offScreenFrameBuf.renderPass));

	std::array<VkImageView, 4> attachments;
	attachments[0] = offScreenFrameBuf.position.view;
	attachments[1] = offScreenFrameBuf.normal.view;
	attachments[2] = offScreenFrameBuf.albedo.view;
	attachments[3] = offScreenFrameBuf.depth.view;

	VkFramebufferCreateInfo fbufCreateInfo = {};
	fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbufCreateInfo.pNext = NULL;
	fbufCreateInfo.renderPass = offScreenFrameBuf.renderPass;
	fbufCreateInfo.pAttachments = attachments.data();
	fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	fbufCreateInfo.width = offScreenFrameBuf.width;
	fbufCreateInfo.height = offScreenFrameBuf.height;
	fbufCreateInfo.layers = 1;
	VK_CHECK_RESULT(vkCreateFramebuffer(device, &fbufCreateInfo, nullptr, &offScreenFrameBuf.frameBuffer));

	// Create sampler to sample from the color attachments
	VkSamplerCreateInfo sampler = vks::initializers::samplerCreateInfo();
	sampler.magFilter = VK_FILTER_NEAREST;
	sampler.minFilter = VK_FILTER_NEAREST;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.addressModeV = sampler.addressModeU;
	sampler.addressModeW = sampler.addressModeU;
	sampler.mipLodBias = 0.0f;
	sampler.maxAnisotropy = 1.0f;
	sampler.minLod = 0.0f;
	sampler.maxLod = 1.0f;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	VK_CHECK_RESULT(vkCreateSampler(device, &sampler, nullptr, &colorSampler));
}

int g_a = 0;
int& r_a = g_a;
const int& a()
{
	return r_a;
}

bool VulkanRenderer::Update()
{
	VulkanRenderPass<TestRenderPass> m_vulkanRenderPass;

	using a = TestRenderPass::RenderPassInstance::GetUVType<0>::Type;

	m_vulkanRenderPass.RegisterRenderPassInstance(
		m_renderWindow->GetVulkanWindowInfo(), 
		m_renderWindow->GetVulkanContext(),
		m_renderWindow->GetVulkanContext()->m_Device,
		*g_instance);

	for(auto rp : m_renderPassManager.m_renderPasses)
	{
	    for(auto c : m_renderPassManager.m_renderPassInstanceContainers)
	    {
	        if(c->GetId() == rp)
	        {
	            //render c ?
	        }
	    }
	}

	return true;
}

void VulkanRenderer::CleanupRenderer()
{
}

void VulkanRenderer::CreateDisplayBuffers()
{
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageCreateInfo.extent.width = m_viewPortExtents.x;
    imageCreateInfo.extent.height = m_viewPortExtents.y;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkMemoryAllocateInfo memAlloc{};
    VkMemoryRequirements memReqs;

    VkDevice device = m_renderWindow->GetVulkanContext()->m_Device;

    m_offscreenBuffer.m_color = new BLA::TextureView();

    VkResult r = VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &m_offscreenBuffer.m_color->m_image));
    if(r != VkResult::VK_SUCCESS)
    {
        return;
    }
    vkGetImageMemoryRequirements(device, m_offscreenBuffer.m_color->m_image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = m_renderWindow->GetVulkanContext()->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    r = VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &m_offscreenBuffer.m_color->m_memory));
    if (r != VkResult::VK_SUCCESS)
    {
        return;
    }
    r = VK_CHECK_RESULT(vkBindImageMemory(device, m_offscreenBuffer.m_color->m_image, m_offscreenBuffer.m_color->m_memory, 0));
    if (r != VkResult::VK_SUCCESS)
    {
        return;
    }

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

    r = VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &m_offscreenBuffer.m_color->m_imageView));
    if (r != VkResult::VK_SUCCESS)
    {
        return;
    }
}

#endif