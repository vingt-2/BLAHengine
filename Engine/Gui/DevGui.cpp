// Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "DevGuiManager.h"
#include "DevGuiWindow.h"
#include "DevGuiElements.h"
#include "DevGuiMenu.h"
#include "DevGuiConsole.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "System/Vulkan/Context.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "Renderer/Gpu/Resource.h"
#include "Renderer/Gpu/Image.h"

#include "System/RenderWindow.h"
#include "System/InputManager.h"
#include "System/FileSystem/Files.h"
#include "System/Console.h"
#include "Core/Scene.h"

#include "EngineInstance.h"

#include <iomanip>

#include "GLFW/glfw3.h"

using namespace BLA;
using namespace System;

BLA_IMPLEMENT_SINGLETON(DevGuiManager);

blaBool IsItemDoubleCliked(int mouse_button = 0);

void BLAengineStyleColors(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.92f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.33f, 0.33f, 0.33f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.52f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.82f, 0.52f, 0.20f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.98f, 0.63f, 0.26f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.82f, 0.52f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.98f, 0.63f, 0.26f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.98f, 0.65f, 0.26f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.40f, 0.49f, 0.31f, 0.93f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.41f, 0.85f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.0f);
    //colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.22f);
    //colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 18.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 12.0f);
}

ImFont* f;
ImFont* f2;

DevGuiManager::DevGuiManager(RenderWindow* glfwWindow) :
    m_window(glfwWindow)
{
    Init();
    m_lastFileBrowserOpenDirectory = "./";
}

DevGuiManager::~DevGuiManager()
{
    Destroy();
}

DevGuiWindow* DevGuiManager::OpenWindow(blaString name)
{
    auto w = m_openWindows.find(name);
    if (w != m_openWindows.end())
        return w->second;
    
    m_openWindows.insert(blaPair<blaString, DevGuiWindow*>(name, new DevGuiWindow(name, blaIVec2(10, 10))));
    return m_openWindows[name];
}

DevGuiWindow* DevGuiManager::GetWindow(blaString name)
{
    auto w = m_openWindows.find(name);
    if (w != m_openWindows.end())
        return w->second;
    return nullptr;
}

void DevGuiManager::OpenWindow(blaString name, DevGuiWindow* window)
{
    m_openWindows.insert(blaPair<blaString, DevGuiWindow*>(name, window));
}

static void CreateImGuiVulkanRenderPass(const Vulkan::Context* vulkanInterface, Vulkan::WindowInfo* vulkanWindowInfo)
{
    VkAttachmentDescription attachment = {};
    attachment.format = vulkanWindowInfo->m_surfaceFormat.format;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = vulkanWindowInfo->m_clearEnable ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    VkResult err = vkCreateRenderPass(vulkanInterface->m_device, &info, nullptr, &vulkanWindowInfo->m_renderWindowPresentationPass);
    Vulkan::Context::HandleError(err);
}

void DevGuiManager::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    f = io.Fonts->AddFontFromFileTTF("./resources/fonts/roboto-Light.ttf", 16.0f);
    f2 = io.Fonts->AddFontFromFileTTF("./resources/fonts/roboto-thin.ttf", 18.0f);

    // Setup Dear ImGui style
    BLAengineStyleColors(nullptr);
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    GLFWVulkanRenderWindow* renderWindow = static_cast<GLFWVulkanRenderWindow*>(m_window);

    ImGui_ImplGlfw_InitForVulkan(renderWindow->GetWindowPointer(), true);

    const Vulkan::Context* vulkanInterface = renderWindow->GetVulkanInterface();
    Vulkan::WindowInfo* renderWindowInfo = renderWindow->GetVulkanWindowInfo();

    CreateImGuiVulkanRenderPass(vulkanInterface, renderWindowInfo);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkanInterface->m_instance;
    init_info.PhysicalDevice = vulkanInterface->m_physicalDevice;
    init_info.Device = vulkanInterface->m_device;
    init_info.QueueFamily = vulkanInterface->m_queueFamily;
    init_info.Queue = vulkanInterface->m_queue;
    init_info.PipelineCache = vulkanInterface->m_pipelineCache;
    init_info.DescriptorPool = vulkanInterface->m_descriptorPool;
    init_info.Allocator = vulkanInterface->m_allocator;
    init_info.MinImageCount = vulkanInterface->m_minImageCount;
    init_info.ImageCount = vulkanInterface->m_minImageCount;
    init_info.CheckVkResultFn = &Vulkan::Context::HandleError;
    ImGui_ImplVulkan_Init(&init_info, renderWindowInfo->m_renderWindowPresentationPass);
    {
        // Use any command queue
        VkCommandPool command_pool = renderWindowInfo->m_frames[renderWindowInfo->m_frameIndex].m_commandPool;
        VkCommandBuffer command_buffer = renderWindowInfo->m_frames[renderWindowInfo->m_frameIndex].m_commandBuffer;

        VkResult err = vkResetCommandPool(vulkanInterface->m_device, command_pool, 0);
        Vulkan::Context::HandleError(err);
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(command_buffer, &begin_info);
        Vulkan::Context::HandleError(err);

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        err = vkEndCommandBuffer(command_buffer);
        Vulkan::Context::HandleError(err);
        err = vkQueueSubmit(vulkanInterface->m_queue, 1, &end_info, VK_NULL_HANDLE);
        Vulkan::Context::HandleError(err);

        err = vkDeviceWaitIdle(vulkanInterface->m_device);
        Vulkan::Context::HandleError(err);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void DevGuiManager::Destroy()
{
    for (auto window : m_openWindows)
    {
        delete window.second;
    }

    // Cleanup
    GLFWVulkanRenderWindow* renderWindow = static_cast<GLFWVulkanRenderWindow*>(m_window);

    const Vulkan::Context* vulkanInterface = renderWindow->GetVulkanInterface();
    Vulkan::WindowInfo* renderWindowInfo = renderWindow->GetVulkanWindowInfo();
    vkDestroyRenderPass(vulkanInterface->m_device, renderWindowInfo->m_renderWindowPresentationPass, nullptr);

    ImGui_ImplVulkan_Shutdown();

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DevGuiElement::SendEvent(DevGuiElementEventPayload::EventType eventType, void* pEventPayload)
{
    for (const auto& cb : m_registeredCallbacks)
    {
        if ((cb.m_eventTriggerFlags & eventType) != 0)
        {
            DevGuiElementEventPayload callbackPayload{ eventType, pEventPayload };
            cb.m_callback(cb.m_callerPtr, callbackPayload);
        }
    }
}

void DevGuiElement::HandleDragDropOfElements()
{
    struct DragPtrPayload
    {
        DevGuiElement* sender;
    };

    // Our buttons are both drag sources and drag targets here!
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        DragPtrPayload p = { this };
        ImGui::SetDragDropPayload("drag_drop_elements", &p, sizeof(DragPtrPayload));
        ImGui::Text("%s", GetName());
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("drag_drop_elements"))
        {
            IM_ASSERT(payload->DataSize == sizeof(DragPtrPayload));

            DevGuiElement* payloadPtr = (DevGuiElement*)((DragPtrPayload*)payload->Data)->sender;
            BlaDroppablePayload p = { this, payloadPtr };
            SendEvent(DevGuiElementEventPayload::EventType::ELEMENT_DROPPED, (void*)&p);
        }
        ImGui::EndDragDropTarget();
    }
}

void DevGuiElement::RegisterEvents(DevGuiRegisteredEvents& cb)
{
    blaVector<DevGuiRegisteredEvents>::iterator it = std::find_if(m_registeredCallbacks.begin(), m_registeredCallbacks.end(),
        [cb](const DevGuiRegisteredEvents& c) {return cb.m_callback == c.m_callback; });

    if (it == m_registeredCallbacks.end())
    {
        m_registeredCallbacks.push_back(cb);
    }
}

void DevGuiElement::UnRegisterEvents(DevGuiRegisteredEvents& cb)
{
    blaVector<DevGuiRegisteredEvents>::iterator it = std::find_if(m_registeredCallbacks.begin(), m_registeredCallbacks.end(),
        [cb](const DevGuiRegisteredEvents& c) {return cb.m_callback == c.m_callback; });

    if (it != m_registeredCallbacks.end())
    {
        m_registeredCallbacks.erase(it);
    }
}

void DevGuiElement::Render()
{
    auto child = GetChild();

    while (child != nullptr)
    {
        child->Render();
        child = child->GetNext();
    }
}

DevGuiCollapsibleElement::DevGuiCollapsibleElement(const blaString& name, blaStringId groupId): DevGuiElement(
    name, groupId)
{
}

void DevGuiCollapsibleElement::Render()
{
    int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (m_decorateHeader ? ImGuiTreeNodeFlags_Framed : 0);

    if (!m_child)
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if(m_isSelected) 
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    if (ImGui::TreeNodeEx((void*)(intptr_t)this, flags, GetName().c_str()))
    {
        if (IsItemDoubleCliked(ImGuiMouseButton_Left))
        {
            SendEvent(DevGuiElementEventPayload::EventType::DOUBLE_CLICKED, this);
        }
        else if (ImGui::IsItemClicked()) 
        {
            SendEvent(DevGuiElementEventPayload::EventType::SELECTED, this);
        }

        HandleDragDropOfElements();
        
        auto child = GetChild();

        while (child != nullptr)
        {
            child->Render();
            child = child->GetNext();
        }

        ImGui::TreePop();
    }
    else
    {
        if (IsItemDoubleCliked(ImGuiMouseButton_Left))
        {
            SendEvent(DevGuiElementEventPayload::EventType::DOUBLE_CLICKED, this);
        }
        else if (ImGui::IsItemClicked())
        {
            SendEvent(DevGuiElementEventPayload::EventType::SELECTED, this);
        }
        
        HandleDragDropOfElements();
    }
}

void DevGuiCollapsibleHeaderElement::Render()
{
    if (ImGui::CollapsingHeader(GetName().c_str()))
    {
        auto child = GetChild();

        while (child != nullptr)
        {
            child->Render();
            child = child->GetNext();
        }
    }
}

void DevGuiSimpleTextElement::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::Text(m_text.c_str());
    // END OCornut's Dear ImGui Specific Code Now
    HandleDragDropOfElements();

    DevGuiElement::Render();
}

bool BLA::DevGuiEditElementVectorPreRender(DevGuiElement* element)
{
    int flags = ImGuiTreeNodeFlags_OpenOnArrow;

    return ImGui::TreeNodeEx((void*)(intptr_t)element, flags, element->GetName().c_str());
}

void BLA::DevGuiEditElementVectorPostRender(DevGuiElement* element)
{
    ImGui::TreePop();
}

template<>
void DevGuiEditElement<bool>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    bool value = *m_pToValue;
    ImGui::Checkbox(("##" + GetName()).c_str(), &value);
    ImGui::Columns(1);
    if (value != *m_pToValue)
    {
        m_onEditFunctor(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(m_pToValue), sizeof(bool));
    }
}

template<>
void DevGuiEditElement<blaF32>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    blaF32 value = *m_pToValue;
    ImGui::InputFloat(("##" + GetName()).c_str(), &value, 0.1f, 1.f, "%.7f");
    ImGui::Columns(1);
    if(value != *m_pToValue)
    {
        m_onEditFunctor(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(m_pToValue), sizeof(blaF32));
    }
}

template<>
void DevGuiEditElement<blaF64>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    blaF64 value = *m_pToValue;
    ImGui::InputDouble(("##" + GetName()).c_str(), &value, 0.1, 1.0);
    ImGui::Columns(1);
    if (value != *m_pToValue)
    {
        m_onEditFunctor(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(m_pToValue), sizeof(blaF64));
    }
}

template<>
void DevGuiEditElement<blaS32>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    blaS32 value = *m_pToValue;
    ImGui::InputInt(("##" + GetName()).c_str(), &value, 1, 10);
    ImGui::Columns(1);
    if (value != *m_pToValue)
    {
        m_onEditFunctor(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(m_pToValue), sizeof(blaS32));
    }
}

template<>
void DevGuiEditElement<blaVec2>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    blaVec2 value = *m_pToValue;
    ImGui::InputFloat2(("##" + GetName()).c_str(), &(value.x));
    ImGui::Columns(1);
    if (value != *m_pToValue)
    {
        m_onEditFunctor(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(m_pToValue), sizeof(blaVec2));
    }
}

template<>
void DevGuiEditElement<blaVec3>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    blaVec3 value = *m_pToValue;
    ImGui::InputFloat3(("##" + GetName()).c_str(), &(value.x));
    ImGui::Columns(1);
    if (value != *m_pToValue)
    {
        m_onEditFunctor(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(m_pToValue), sizeof(blaVec3));
    }
}

template<>
void DevGuiEditElement<blaQuat>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    blaQuat value = *m_pToValue;
    ImGui::InputFloat4(("##" + GetName()).c_str(), &(value.x));
    ImGui::Columns(1);
    if (value != *m_pToValue)
    {

        m_onEditFunctor(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(m_pToValue), sizeof(blaQuat));
    }
}

template<>
void DevGuiEditElement<blaPosQuat>::Render()
{
    ImGui::Text(GetName().c_str());
    ImGui::Columns(2, 0, false);
    ImGui::Text("\tPosition"); ImGui::NextColumn();
    blaVec3 posValue = m_pToValue->GetTranslation3();
    ImGui::InputFloat3(("##P" + GetName()).c_str(), &(posValue.x)); ImGui::NextColumn();
    blaQuat quatValue = m_pToValue->GetRotation();
    ImGui::Text("\tRotation"); ImGui::NextColumn();
    ImGui::InputFloat4(("##R" + GetName()).c_str(), &(quatValue.x)); ImGui::NextColumn();
    ImGui::Columns(1);
    if (posValue != m_pToValue->GetTranslation3() || quatValue != m_pToValue->GetRotation())
    {
        blaPosQuat pq(posValue, quatValue);
        
        m_onEditFunctor(reinterpret_cast<char*>(&pq), reinterpret_cast<char*>(m_pToValue), sizeof(blaPosQuat));
    }
}

template<>
void DevGuiEditElement<blaScaledTransform>::Render()
{
    ImGui::Text(GetName().c_str());
    ImGui::Columns(2, 0, false);
    ImGui::Text("\tPosition"); ImGui::NextColumn();
    blaVec3 posValue = m_pToValue->m_posQuat.GetTranslation3();
    ImGui::InputFloat3(("##P" + GetName()).c_str(), &posValue.x); ImGui::NextColumn();
    ImGui::Text("\tRotation"); ImGui::NextColumn();
    blaQuat rotValue = m_pToValue->m_posQuat.GetRotation();
    ImGui::InputFloat4(("##R" + GetName()).c_str(), &rotValue.x); ImGui::NextColumn();
    ImGui::Text("\tScale"); ImGui::NextColumn();
    blaVec3 scaleValue = m_pToValue->m_scale;
    ImGui::InputFloat3(("##S" + GetName()).c_str(), &scaleValue.x); ImGui::NextColumn();
    ImGui::Columns(1);

    if (posValue != m_pToValue->m_posQuat.GetTranslation3() ||
        rotValue != m_pToValue->m_posQuat.GetRotation() ||
        scaleValue != m_pToValue->GetScale())
    {
        blaScaledTransform t(scaleValue, blaPosQuat(posValue, rotValue));

        m_onEditFunctor(reinterpret_cast<char*>(&t), reinterpret_cast<char*>(m_pToValue), sizeof(blaScaledTransform));
    }
}

template<>
void DevGuiEditElement<blaString>::Render()
{
    char inputBuf[2048];
    strcpy_s(inputBuf, m_pToValue->c_str());
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str()); ImGui::NextColumn();
    ImGui::InputText(("##" + GetName()).c_str(), inputBuf, sizeof(inputBuf));
    ImGui::Columns(1);
    blaString editValue = blaString(inputBuf);
    if(*m_pToValue != editValue)
    {
        m_onEditFunctor(reinterpret_cast<char*>(&editValue), reinterpret_cast<char*>(m_pToValue), sizeof(blaString));
    }
}

template <typename T1, typename T2>
void DevGuiEditElementPair<T1, T2>::Render()
{
    ImGui::Text(GetName().c_str());
    DevGuiEditElement<T1>("First", &m_pToPair->first).Render();
    DevGuiEditElement<T2>("Second", &m_pToPair->second).Render();
}

template<>
void DevGuiEditElement<Core::GameObject>::Render()
{
    ImGui::Columns(2, 0, false);
    ImGui::Text(GetName().c_str());
    ImGui::NextColumn();
    if (m_pToValue->IsValid())
    {
        bool Selected = false;
        ImGui::Selectable(blaString(m_pToValue->GetId()).c_str(), Selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 0));
    }
    else
    {
        ImGui::Text("Invalid Object Reference");
    }
    ImGui::Columns(1);
}

DevGuiWindow::DevGuiWindow():
    m_windowName("")
    , m_windowPosition(blaIVec2(0))
    , m_windowFlags(0)
    , m_rootElement(nullptr)
    , m_menu(nullptr), m_hasFocus(false), m_bOpenWindow(true)
{
}

DevGuiWindow::DevGuiWindow(const blaString& windowName, const blaIVec2& windowPosition):
m_windowName(windowName)
, m_windowPosition(windowPosition)
, m_windowFlags(0)
, m_rootElement(nullptr)
, m_menu(nullptr)
, m_hasFocus(false)
, m_bOpenWindow(true)
{
}

DevGuiWindow::~DevGuiWindow()
{
    delete m_rootElement;
    delete m_menu;
}

void DevGuiWindow::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImVec2 position((float)m_windowPosition.x, (float)m_windowPosition.y);
    ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
    ImGui::Begin(m_windowName.c_str(), &m_bOpenWindow, m_windowFlags);
    // END OCornut's Dear ImGui Specific Code Now

    if(m_menu)
    {
        m_menu->Render();
    }
    
    if (m_rootElement)
    {
        m_rootElement->Render();
    }

    m_hasFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_None);
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::End();
    // END OCornut's Dear ImGui Specific Code Now
}

DevGuiElement* DevGuiWindow::RootElement() const
{
    return m_rootElement;
}

void DevGuiWindow::SetRootElement(DevGuiElement* imGuiElements)
{
    m_rootElement = imGuiElements;
}

bool DevGuiWindow::HasFocus() const
{
    return m_hasFocus;
}

DevGuiMenu& DevGuiWindow::AddMenu()
{
    m_menu = new DevGuiMenu();
    return *m_menu;
}

BlaOneTimeWindow::BlaOneTimeWindow(): DevGuiWindow()
{
}

BlaOneTimeWindow::BlaOneTimeWindow(const blaString& windowName, const blaIVec2& windowPosition):
    DevGuiWindow(windowName, windowPosition)
{
}

void BlaOneTimeWindow::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImVec2 position((float)m_windowPosition.x, (float)m_windowPosition.y);
    ImGui::SetNextWindowPos(position);
    ImGui::Begin(m_windowName.c_str(), NULL, m_windowFlags | DevGuiWindow::WindowFlags::MenuBar | ImGuiWindowFlags_NoTitleBar);
    // END OCornut's Dear ImGui Specific Code Now

    if (m_rootElement)
    {
        m_rootElement->Render();
    }

    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::End();
    // END OCornut's Dear ImGui Specific Code Now
}

struct BLA::RenderWindowData
{
    RenderWindowData()
    {
        memset(this, 0, sizeof(RenderWindowData));
    }

    ImTextureID m_offscreenImageTextureId;

    //TODO: For now, devgui.cpp speaks vulkan ... so it's cool to do this, FOR NOW
    VkImage m_currentImage;
    VkImageView m_currentImageView;
    VkSampler m_currentRenderTargetSampler;
};


void DevGuiRenderViewportWindow::UpdateDisplayTexture(VulkanRenderer* renderer)
{
    const GLFWVulkanRenderWindow* renderWindow = dynamic_cast<const GLFWVulkanRenderWindow*>(renderer->GetRenderWindow());

    VkImage offscreenBufferImage = static_cast<VkImage>(renderer->m_offscreenBuffer.m_color->GetHandle().m_impl.pointer);

    if (offscreenBufferImage == m_renderData->m_currentImage)
        return;

    if (!renderer->m_offscreenBuffer.m_color->m_allocationHandle.pointer)
        return;

    m_renderData->m_currentImage = offscreenBufferImage;

    VkDevice device = renderWindow->GetVulkanInterface()->m_device;

    if (m_renderData->m_currentImageView)
        vkDestroyImageView(device, m_renderData->m_currentImageView, nullptr);

    if (m_renderData->m_currentRenderTargetSampler)
        vkDestroySampler(device, m_renderData->m_currentRenderTargetSampler, nullptr);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_renderData->m_currentImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(device, &viewInfo, nullptr, &m_renderData->m_currentImageView);

    // assert renderWindow

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    vkCreateSampler(renderWindow->GetVulkanInterface()->m_device, &samplerInfo, nullptr, &m_renderData->m_currentRenderTargetSampler);

    m_renderData->m_offscreenImageTextureId = ImGui_ImplVulkan_AddTexture(m_renderData->m_currentRenderTargetSampler, m_renderData->m_currentImageView, VK_IMAGE_LAYOUT_GENERAL);
}

DevGuiRenderViewportWindow::DevGuiRenderViewportWindow(VulkanRenderer* renderer, const blaString& windowName, const blaIVec2& windowPosition):
    DevGuiWindow(windowName, windowPosition), m_pRenderer(renderer), m_cursorScreenSpacePosition(), m_renderData(new RenderWindowData())
{
    UpdateDisplayTexture(renderer);
}

void DevGuiRenderViewportWindow::Render()
{
    if (VulkanRenderer* renderer = dynamic_cast<VulkanRenderer*>(m_pRenderer))
    {
        UpdateDisplayTexture(renderer);   

        // BEGIN OCornut's Dear ImGui Specific Code Now
        ImVec2 position((float)m_windowPosition.x, (float)m_windowPosition.y);
        ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
        ImGui::Begin(m_windowName.c_str(), &m_bOpenWindow, m_windowFlags);

        ImVec2 windowPos = ImGui::GetWindowPos();

        if (m_renderData->m_offscreenImageTextureId)
        {
            ImGui::Image(m_renderData->m_offscreenImageTextureId, ImGui::GetWindowSize());
        }

        ImVec2 cursorInWindow = ImGui::GetCursorPos();

        m_pRenderer->SetViewportSize(blaIVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));

        ImVec2 mouse = ImGui::GetMousePos();

        m_cursorScreenSpacePosition = blaVec2(
            1.0f - (mouse.x - windowPos.x) / ImGui::GetWindowWidth(),
            1.0f - (mouse.y - windowPos.y) / ImGui::GetWindowHeight());

        m_hasFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_None);

        ImGui::End();
    }
}

blaVec2 DevGuiRenderViewportWindow::GetMousePointerScreenSpaceCoordinates() const
{
    return m_cursorScreenSpacePosition;
}

bool g_show_demo_window = false;
bool g_debugFileBrowser = false;

static void FrameRender(const Vulkan::Context* vulkanInterface, Vulkan::WindowInfo* vulkanWindowInfo)
{
    VkResult err;

    VkSemaphore imageAcquiredSemaphore = vulkanWindowInfo->m_frameSemaphores[vulkanWindowInfo->m_semaphoreIndex].m_imageAcquiredSemaphore;
    VkSemaphore renderCompleteSemaphore = vulkanWindowInfo->m_frameSemaphores[vulkanWindowInfo->m_semaphoreIndex].m_renderCompleteSemaphore;
    err = vkAcquireNextImageKHR(vulkanInterface->m_device, vulkanWindowInfo->m_swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &vulkanWindowInfo->m_frameIndex);
    
    Vulkan::Context::HandleError(err);

    Vulkan::FrameContext* fd = &vulkanWindowInfo->m_frames[vulkanWindowInfo->m_frameIndex];
    {
        err = vkWaitForFences(vulkanInterface->m_device, 1, &fd->m_imageFence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
        Vulkan::Context::HandleError(err);

        err = vkResetFences(vulkanInterface->m_device, 1, &fd->m_imageFence);
        Vulkan::Context::HandleError(err);
    }
    {
        err = vkResetCommandPool(vulkanInterface->m_device, fd->m_commandPool, 0);
        Vulkan::Context::HandleError(err);
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->m_commandBuffer, &info);
        Vulkan::Context::HandleError(err);
    }
    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = vulkanWindowInfo->m_renderWindowPresentationPass;
        info.framebuffer = fd->m_framebuffer;
        info.renderArea.extent = vulkanWindowInfo->m_extent;
        info.clearValueCount = 1;
        info.pClearValues = &vulkanWindowInfo->m_clearValue;
        vkCmdBeginRenderPass(fd->m_commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record Imgui Draw Data and draw funcs into command buffer
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->m_commandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->m_commandBuffer);
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &imageAcquiredSemaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &fd->m_commandBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &renderCompleteSemaphore;

        err = vkEndCommandBuffer(fd->m_commandBuffer);
        Vulkan::Context::HandleError(err);
        err = vkQueueSubmit(vulkanInterface->m_queue, 1, &info, fd->m_imageFence);
        Vulkan::Context::HandleError(err);
    }
}

void DevGuiManager::Update(bool editorBuild)
{
    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    if(editorBuild)
    {
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    
    ImGui::NewFrame();

    if (editorBuild)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;// | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        bool p_open;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Dockspace", &p_open, window_flags);

        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        }
        ImGui::End();
    }

    ImGui::PushFont(f); // PopFont()

    m_menuBar.Render();

    blaVector<blaString> toClose;
    for (auto& window : m_openWindows)
    {
        window.second->Render();
        if (!window.second->m_bOpenWindow)
        {
            toClose.push_back(window.first);
        }
    }

    for (auto& window : toClose)
    {
        delete m_openWindows[window];
        m_openWindows.erase(window);
    }

    for (auto& window : m_openBrowsers)
    {
        window.second->Render();
    }

    const InputManager* inputs = InputManager::GetSingletonInstanceRead();
    // Display the Dear ImGui toolkit helper so we never have to look too long to know what we can get done !
    if (inputs->GetKeyState(BLA_KEY_LEFT_SHIFT).IsDown() &&
        inputs->GetKeyState(BLA_KEY_LEFT_ALT).IsDown() &&
        inputs->GetKeyState(BLA_KEY_1).IsRisingEdge())
    {
        g_show_demo_window = !g_show_demo_window;
    }
    if (g_show_demo_window)
    {
        ImGui::ShowDemoWindow(&g_show_demo_window);
    }

    ImGui::PopFont();

    ImGui::Render();

    GLFWVulkanRenderWindow* renderWindow = static_cast<GLFWVulkanRenderWindow*>(m_window);
    // glfwGetFramebufferSize(renderWindow->GetWindowPointer(), &display_w, &display_h);
    
    ImDrawData* draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    if (!is_minimized)
    {
        FrameRender(renderWindow->GetVulkanInterface(), renderWindow->GetVulkanWindowInfo());
    }

    // TODO: Handle viewports from within bla itself ... (This is prone to a lot of issues) (+ non portable obv...)
    // Update and Render additional Platform Windows
   // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
   //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

blaBool DevGuiManager::IsMouseOverGui() const
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

void DevGuiManager::DrawText(const blaString& textToDraw, blaIVec2 renderWindowPosition)
{
}

void DevGuiManager::OpenConsole(const blaString& consoleName)
{
    m_openWindows.insert(blaPair<blaString, DevGuiWindow*>(consoleName, new DevGuiWindow(consoleName, blaIVec2(10, 10))));
    m_openWindows[consoleName]->SetRootElement(new DevGuiConsole(consoleName, Console::GetSingletonInstance()));
}

OpenFilePrompt* DevGuiManager::CreateOpenFilePrompt(blaString browserName, blaBool disableMultipleSelection)
{
    OpenFilePrompt* browser = new OpenFilePrompt(browserName, m_lastFileBrowserOpenDirectory, "./", disableMultipleSelection);

    auto browserSearch = m_openBrowsers.find(browserName);
    if (browserSearch != m_openBrowsers.end())
    {
        return dynamic_cast<OpenFilePrompt*>(browserSearch->second);
    }

    m_openBrowsers.insert(blaPair<blaString, BlaFileBrowser*>(browserName, browser));
    return (OpenFilePrompt*)m_openBrowsers.at(browserName);
}

SaveFilePrompt* DevGuiManager::CreateSaveFilePrompt(blaString browserName)
{
    SaveFilePrompt* browser = new SaveFilePrompt(browserName, m_lastFileBrowserOpenDirectory, "./");

    auto browserSearch = m_openBrowsers.find(browserName);
    if (browserSearch != m_openBrowsers.end())
    {
        return dynamic_cast<SaveFilePrompt*>(browserSearch->second);
    }

    m_openBrowsers.insert(blaPair<blaString, BlaFileBrowser*>(browserName, browser));
    return (SaveFilePrompt*)m_openBrowsers.at(browserName);
}


blaBool DevGuiManager::CloseFileBrowser(blaString browserName)
{
    auto browserSearch = m_openBrowsers.find(browserName);
    if (browserSearch != m_openBrowsers.end())
    {
        m_lastFileBrowserOpenDirectory = browserSearch->second->m_currentFilesDirectory;

        delete browserSearch->second;

        m_openBrowsers.erase(browserSearch);
    }
    return false;
}

DevGuiMenuItem::DevGuiMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator):
    m_name(name)
    , m_switch(bool_switch)
    , m_endWithSeparator(endWithSeparator) {}

void DevGuiMenuItem::Render()
{
    ImGui::MenuItem(m_name.c_str(), NULL, m_switch);

    if (m_endWithSeparator)
    {
        ImGui::Separator();
    }
}

DevGuiMenuTab::DevGuiMenuTab(blaString name): m_name(name) {}

DevGuiMenuTab::~DevGuiMenuTab()
{
    for (DevGuiMenuBase* m : m_menuItems) delete m;
}

void DevGuiMenuTab::AddMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator)
{
    m_menuItems.emplace_back(new DevGuiMenuItem(name, bool_switch, endWithSeparator));
}

DevGuiMenuTab& DevGuiMenuTab::AddSubMenu(blaString name)
{
    DevGuiMenuTab* tab = new DevGuiMenuTab(name);
    m_menuItems.emplace_back(tab);
    return *tab;
}

void DevGuiMenuTab::Render()
{
    if (ImGui::BeginMenu(m_name.c_str(), true))
    {
        for (int i = 0; i < m_menuItems.size(); ++i)
        {
            m_menuItems[i]->Render();
        }
        ImGui::EndMenu();
    }
}

void DevGuiMenu::Render()
{
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::PushFont(f2);
        ImGui::Text("BLAengine");
        ImGui::PopFont();
        for (int i = 0; i < m_menuTabs.size(); ++i)
        {
            m_menuTabs[i]->Render();
        }

        ImGui::EndMainMenuBar();
    }
}

DevGuiMenu::~DevGuiMenu()
{
    for (DevGuiMenuBase* m : m_menuTabs) delete m;
}

void DevGuiMenu::AddMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator)
{
    m_menuTabs.emplace_back(new DevGuiMenuItem(name, bool_switch, endWithSeparator));
}

DevGuiMenuTab& DevGuiMenu::AddSubMenu(blaString name)
{
    DevGuiMenuTab* tab = new DevGuiMenuTab(name);
    m_menuTabs.emplace_back(tab);
    return *tab;
}

void BlaFileBrowser::CurrentFolderGoBack()
{
    auto lastSlashPos = m_currentFilesDirectory.substr(0, m_currentFilesDirectory.length() - 1).find_last_of('/');
    m_currentFilesDirectory = m_currentFilesDirectory.substr(0, lastSlashPos + 1);
}

blaString StringSanitize(blaString in)
{
    blaString out;
    for (char c : in)
    {
        if (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '.' || c == '-' || c == '_')
        {
            out += c;
        }
    }

    return out;
}

BlaFileBrowser::BlaFileBrowser(blaString browserName, blaString filesDirectory, blaString directoryDirectory):
    m_name(browserName)
    , m_currentFilesDirectory(filesDirectory)
    , m_currentDirectoriesDirectory(directoryDirectory)
    , m_currentState(BROWSING_FIRST_RENDER), m_disableMultipleSelection(false)
{
}

void BlaFileBrowser::Render()
{
    blaBool open = true;
    ImGui::Begin(m_name.c_str(), &open, 0);

    if (!open)
    {
        m_currentState = CANCELLED;
        return;
    }

    ImGui::Text("Current Directory: "); ImGui::SameLine();

    char readonlyTextCpy[500];
    strcpy_s(readonlyTextCpy, m_currentFilesDirectory.c_str());
    readonlyTextCpy[m_currentFilesDirectory.size()] = 0;

    ImGui::InputText("", readonlyTextCpy, m_currentFilesDirectory.size() + 1, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
    if (ImGui::Button("Back"))
    {
        CurrentFolderGoBack();
    }
    ImGui::Columns(2);

    if (m_currentState == FileBrowserState::BROWSING_FIRST_RENDER)
    {
        ImGui::SetColumnWidth(0, 150);
        m_currentState = FileBrowserState::BROWSING;
    }

    ImGui::BeginChild("RecursiveDirs", ImVec2(0.f, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    blaVector<DirectoryEntry> dir;
    FileBrowserDisplayDirectoriesRecursive(m_currentDirectoriesDirectory, false);
    ImGui::EndChild();

    ImGui::NextColumn();

    if (ImGui::BeginChild("FolderDisplay", ImVec2(0.f, 300), true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
    {
        FileBrowserDisplayAllContentNonRecursive();
        ImGui::EndChild();
    }

    ImGui::NextColumn();

    ImGui::Columns();

    ImGui::NewLine();
}

OpenFilePrompt::OpenFilePrompt(blaString browserName, blaString filesDirectory, blaString directoryDirectory,
                               blaBool disableMultipleSelection):
    BlaFileBrowser(browserName, filesDirectory, directoryDirectory)
{
    m_disableMultipleSelection = disableMultipleSelection;
}

void OpenFilePrompt::Render()
{
    BlaFileBrowser::Render();

    char readonlyTextCpy[500];

    blaString selectedFiles = "";
    for (auto& file : m_currentSelection)
    {
        selectedFiles += "\"" + file.first + "\" ";
    }

    strcpy_s(readonlyTextCpy, selectedFiles.c_str());
    readonlyTextCpy[selectedFiles.size()] = 0;

    static float selectedFileFormSize = 100.0f; //The 100.0f is just a guess size for the first frame.
    float pos = selectedFileFormSize + 10;
    ImGui::SameLine(ImGui::GetWindowWidth() - pos);

    ImGui::InputText("Selected File(s)", readonlyTextCpy, selectedFiles.size() + 1, ImGuiInputTextFlags_ReadOnly);

    selectedFileFormSize = ImGui::GetItemRectSize().x;


    ImGui::NewLine();

    if (ImGui::BeginChild("ButtonsDisplay", ImVec2(0.f, 0.f), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
    {
        static float cancelButtonSize = 100.0f; //The 100.0f is just a guess size for the first frame.
        float pos = cancelButtonSize + 10;
        ImGui::SameLine(ImGui::GetWindowWidth() - pos);
        if (ImGui::Button("Cancel"))
        {
            m_currentState = FileBrowserState::CANCELLED;
        }
        cancelButtonSize = ImGui::GetItemRectSize().x;

        static float openButtonSize = 100.0f; //The 100.0f is just a guess size for the first frame.
        pos += openButtonSize + 10;
        ImGui::SameLine(ImGui::GetWindowWidth() - pos);
        if (ImGui::Button("Open File"))
        {
            m_currentState = FileBrowserState::CONFIRMED_SELECTION;
        }
        openButtonSize = ImGui::GetItemRectSize().x;

        ImGui::EndChild();
    }

    ImGui::End();
}

SaveFilePrompt::SaveFilePrompt(blaString browserName, blaString filesDirectory, blaString directoryDirectory):
    BlaFileBrowser(browserName, filesDirectory, directoryDirectory)
{
    m_disableMultipleSelection = true;
}

void SaveFilePrompt::Render()
{
    BlaFileBrowser::Render();

    char saveFileTextInput[500];

    blaString selectedFiles = "";
    for (auto& file : m_currentSelection)
    {
        selectedFiles += file.first;
    }

    strcpy_s(saveFileTextInput, selectedFiles.c_str());
    saveFileTextInput[selectedFiles.size()] = 0;

    static float selectedFileFormSize = 100.0f; //The 100.0f is just a guess size for the first frame.
    float pos = selectedFileFormSize + 10;
    ImGui::SameLine(ImGui::GetWindowWidth() - pos);

    bool inputReturn = ImGui::InputText("Save Filename", saveFileTextInput, selectedFiles.size() + 1, ImGuiInputTextFlags_EnterReturnsTrue);

    blaString txtInput = blaString(saveFileTextInput);

    selectedFileFormSize = ImGui::GetItemRectSize().x;

    ImGui::NewLine();

    if (ImGui::BeginChild("ButtonsDisplay", ImVec2(0.f, 0.f), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
    {
        static float cancelButtonSize = 100.0f; //The 100.0f is just a guess size for the first frame.
        float pos = cancelButtonSize + 10;
        ImGui::SameLine(ImGui::GetWindowWidth() - pos);
        if (ImGui::Button("Cancel"))
        {
            m_currentState = FileBrowserState::CANCELLED;
        }
        cancelButtonSize = ImGui::GetItemRectSize().x;

        static float openButtonSize = 100.0f; //The 100.0f is just a guess size for the first frame.
        pos += openButtonSize + 10;
        ImGui::SameLine(ImGui::GetWindowWidth() - pos);

        if (ImGui::Button("Save") || inputReturn)
        {
            if (StringSanitize(txtInput).length() > 0)
            {
                m_currentSavePath = m_currentFilesDirectory + txtInput;
                m_currentState = FileBrowserState::CONFIRMED_SELECTION;
            }
        }
        openButtonSize = ImGui::GetItemRectSize().x;

        ImGui::EndChild();
    }

    ImGui::End();
}

blaBool OpenFilePrompt::GetConfirmedSelection(blaVector<FileEntry>& selection) const
{
    if (m_currentState == FileBrowserState::CONFIRMED_SELECTION)
    {
        for (auto& fileEntry : m_currentSelection)
        {
            selection.push_back(fileEntry.second);
        }
        return true;
    }
    return false;
}

blaBool SaveFilePrompt::GetConfirmedSavePath(blaString& savePath) const
{
    if (m_currentState == FileBrowserState::CONFIRMED_SELECTION)
    {
        savePath = m_currentSavePath;
        return true;
    }
    savePath = "";
    return false;
}


blaBool BlaFileBrowser::IsBrowsingCancelled() const
{
    return m_currentState == CANCELLED;
}

blaBool IsItemDoubleCliked(int mouse_button)
{
    return ImGui::IsMouseDoubleClicked(mouse_button) && ImGui::IsItemHovered(ImGuiHoveredFlags_None);
}

void BlaFileBrowser::FileBrowserDisplayAllContentNonRecursive()
{
    blaVector<DirectoryEntry> dirContent;
    GetAllContentInDirectory(dirContent, m_currentFilesDirectory);

    ImGui::BeginChild("HeaderDisplay", ImVec2(0.f, 20.f), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
    ImGui::Columns(4, (m_name + blaString("FBDACNR_title")).c_str(), false);

    ImGui::Text("Name"); ImGui::NextColumn(); ImGui::Text("Date Modified"); ImGui::NextColumn();
    ImGui::Text("Type"); ImGui::NextColumn(); ImGui::Text("Size"); ImGui::NextColumn();

    ImGui::EndChild();

    ImGui::Separator();
    ImGui::BeginChild("FileAndDirectoryBrowserDisplay", ImVec2(0.f, 0.f), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::Columns(4, (m_name + blaString("FBDACNR")).c_str(), false);

    for (int i = 0; i < dirContent.size(); i++)
    {
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;


        if (m_currentSelection.find(dirContent[i].m_name + dirContent[i].m_extension) != m_currentSelection.end())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, (dirContent[i].m_name + dirContent[i].m_extension).c_str()); ImGui::NextColumn();

        if (ImGui::IsItemClicked())
        {
            ImGuiIO& io = ImGui::GetIO();
            if (m_disableMultipleSelection || !io.KeyCtrl)
            {
                m_currentSelection.clear();
            }

            if (dirContent[i].m_entryType == DirectoryEntry::DIRECTORY)
            {
                // If we double click on a file, we either go in the folder or go back if ".."
                if (IsItemDoubleCliked())
                {
                    if (dirContent[i].m_name != "..")
                    {
                        m_currentFilesDirectory += dirContent[i].m_name + "/";
                    }
                    else
                    {
                        CurrentFolderGoBack();
                    }
                }
            }
            else if (dirContent[i].m_entryType == DirectoryEntry::REGULAR_FILE)
            {
                m_currentSelection.insert(blaPair<blaString, FileEntry>(dirContent[i].m_name + dirContent[i].m_extension, dirContent[i]));

                // If we double click on a file, we confirm selection.
                if (IsItemDoubleCliked())
                {
                    m_currentState = CONFIRMED_SELECTION;
                }
            }
        }

        ImGui::Text(BlaFileTimeToString(dirContent[i].m_lastEditTime).c_str()); ImGui::NextColumn();
        ImGui::Text(dirContent[i].m_extension.c_str()); ImGui::NextColumn();

        if (dirContent[i].m_entryType == DirectoryEntry::REGULAR_FILE)
        {
            ImGui::Text(BlaFileSizeToString(dirContent[i].m_size).c_str());
        }
        ImGui::NextColumn();

    }
    ImGui::EndChild();
}

void BlaFileBrowser::FileBrowserDisplayDirectoriesRecursive(blaString currentdirectory, blaBool displayEditTime)
{
    blaVector<DirectoryEntry> dirContent;

    GetDirectoriesInDirectory(dirContent, currentdirectory);

    if (displayEditTime)
    {
        ImGui::Columns(2, (m_name + blaString("FBDDR")).c_str(), false);
    }

    for (int i = 0; i < dirContent.size(); i++)
    {
        blaString currentDirectory = currentdirectory + dirContent[i].m_name + "/";
        //Todo : Dont get entries twice ... No time here but to refactor.
        blaVector<DirectoryEntry> childEntries;
        GetDirectoriesInDirectory(childEntries, currentDirectory);

        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (childEntries.empty())
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf;
        }

        if (m_currentFilesDirectory == currentDirectory)
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        // Node
        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, dirContent[i].m_name.c_str());

        if (displayEditTime)
        {
            ImGui::NextColumn();
            ImGui::Text(BlaFileTimeToString(dirContent[i].m_lastEditTime).c_str());
            ImGui::NextColumn();
        }

        if (IsItemDoubleCliked() || (childEntries.empty() && ImGui::IsItemClicked()))
            m_currentFilesDirectory = currentDirectory;

        if (node_open)
        {
            FileBrowserDisplayDirectoriesRecursive(currentDirectory, displayEditTime);
            ImGui::TreePop();
        }
    }
}

//void BlaFileBrowser::FileBrowserDisplayAllContentRecursive()
//{
//    static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
//    int node_clicked = -1;
//
//    blaVector<DirectoryEntry> dirContent;
//
//    GetAllContentInDirectory(dirContent, m_currentFilesDirectory);
//
//    ImGui::Columns(4, (m_name + blaString("FBDACR")).c_str(), true);  // 3-ways, no border
//    //ImGui::Separator();
//    for (int i = 0; i < dirContent.size(); i++)
//    {
//        // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
//        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
//        if (dirContent[i].m_entryType == DirectoryEntry::DIRECTORY)
//        {
//            // Node
//            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, dirContent[i].m_name.c_str()); ImGui::NextColumn();
//
//            ImGui::Text(BlaFileTimeToString(dirContent[i].m_lastEditTime).c_str()); ImGui::NextColumn();
//            ImGui::Text("Folder"); ImGui::NextColumn(); ImGui::NextColumn();
//
//            if (ImGui::IsItemClicked())
//                node_clicked = i;
//
//            if (node_open)
//            {
//                FileBrowserDisplayAllContentRecursive();
//                ImGui::TreePop();
//            }
//        }
//        else
//        {
//            // Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
//            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
//            ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, (dirContent[i].m_name + dirContent[i].m_extension).c_str()); ImGui::NextColumn();
//
//            ImGui::Text(BlaFileTimeToString(dirContent[i].m_lastEditTime).c_str()); ImGui::NextColumn();
//            ImGui::Text(dirContent[i].m_extension.c_str()); ImGui::NextColumn();
//            ImGui::Text(BlaFileSizeToString(dirContent[i].m_size).c_str()); ImGui::NextColumn();
//
//            if (ImGui::IsItemClicked())
//            {
//                node_clicked = i;
//            }
//        }
//    }
//}

void DevGuiConsole::Render()
{
    bool copy_to_clipboard = false;
    blaVector<blaString> consoleLines;

    m_pConsoleSingleton->GetLastNLines(m_maxLineCount, consoleLines);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    /*if (copy_to_clipboard)
        ImGui::LogToClipboard();*/

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.5f, 0.5f, 0.5f, 0.3f));
    if (ImGui::BeginChild("ConsoleLogOutput", ImVec2(0, ImGui::GetWindowHeight() - 5.f * ImGui::GetFontSize()), true))
    {
        for (int i = 0; i < consoleLines.size(); i++)
        {
            if (i % 2)
            {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.55f, 0.55f, 0.55f, 0.05f));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.5f, 0.5f, 0.5f, 0.0f));
            }

            char childName[64];
            strcpy_s(childName, (blaString("CL:") + std::to_string(i)).data());
            ImGui::BeginChild(childName, ImVec2(0, ImGui::GetFontSize()), false, ImGuiWindowFlags_NoScrollWithMouse);

            const char* item = consoleLines[i].data();
            /*if (!Filter.PassFilter(item))
                continue;
    */

    // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
            bool pop_color = false;
            if (strstr(item, "[error]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
            else if (strncmp(item, "# ", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
            ImGui::TextUnformatted(item);

            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        ImGui::EndChild();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    // Command-line
    //bool reclaim_focus = false;
    if (ImGui::InputText("Console Input",
        m_pConsoleSingleton->m_currentCommandBuffer,
        2048,
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory,
        &DevGuiConsole::HandleCmdCallbacks,
        (void*)this))
    {
        m_pConsoleSingleton->ExecuteCurrentCommand();
        //reclaim_focus = true;
        strcpy_s(m_pConsoleSingleton->m_currentCommandBuffer, "");
    }

    // Auto-focus on window apparition
    //ImGui::SetItemDefaultFocus();
    //if (reclaim_focus)
    //    ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

}

int DevGuiConsole::HandleCmdCallbacks(ImGuiInputTextCallbackData* data)
{
    Console* pConsole = ((DevGuiConsole*)(data->UserData))->m_pConsoleSingleton;
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
        pConsole->DoCommandCompletion();
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, pConsole->m_currentCommandBuffer);
        break;
    case ImGuiInputTextFlags_CallbackHistory:
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            pConsole->DoCommandHistory(-1);
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, pConsole->m_currentCommandBuffer);
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            pConsole->DoCommandHistory(1);
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, pConsole->m_currentCommandBuffer);
        }
        break;
    default:
        break;
    }
    return 0;
}