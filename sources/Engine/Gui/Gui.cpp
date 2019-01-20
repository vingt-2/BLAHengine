#include "GuiManager.h"
#include "GuiWindow.h"
#include "GuiElements.h"
#include "GuiMenu.h"

#include <External/imgui/imgui.h>
#include <External/imgui/examples/imgui_impl_glfw.h>
#include <External/imgui/examples/imgui_impl_opengl3.h>

#include <Engine/System/RenderWindow.h>
#include <Engine/System/InputManager.h>

#include <memory>

#pragma optimize("", off)

using namespace BLAengine;

void BLAengineStyleColors(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.92f);
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
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.98f, 0.63f, 0.26f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.40f, 0.49f, 0.31f, 0.93f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.60f, 0.73f, 0.88f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.22f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
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
}

ImFont* f;
ImFont* f2;
void BlaGuiManager::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    f = io.Fonts->AddFontFromFileTTF("./resources/fonts/roboto-Light.ttf", 18.0f);
    f2 = io.Fonts->AddFontFromFileTTF("./resources/fonts/roboto-thin.ttf", 19.0f);

    // Setup Dear ImGui style
	BLAengineStyleColors(nullptr);
	//ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

}

void BlaGuiManager::Destroy()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

BlaGuiElement::~BlaGuiElement()
{
    auto child = GetChild();

    while (child != nullptr)
    {
        delete child;
    }
}

void BlaGuiElement::Render()
{
    auto child = GetChild();

    while (child != nullptr)
    {
        child->Render();
    }
}

void BlaGuiTextElement::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::Text(m_text.c_str());
    // END OCornut's Dear ImGui Specific Code Now

    BlaGuiElement::Render();
}

void BlaGuiWindow::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImVec2 position(m_windowPosition.x, m_windowPosition.y);
    ImGui::SetNextWindowPos(position);
    ImGui::Begin(m_windowName.c_str(), NULL, m_windowFlags);
    // END OCornut's Dear ImGui Specific Code Now

    if (m_rootElement)
    {
        m_rootElement->Render();
    }
    
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::End();
    // END OCornut's Dear ImGui Specific Code Now
}

void BlaGuiWindow::SetRootElement(BlaGuiElement* imGuiElements)
{
    delete m_rootElement;
    m_rootElement = imGuiElements;
}

bool g_show_demo_window = false;

void BlaGuiManager::Update()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushFont(f); // PopFont()

    m_menuBar.Render();

    for (auto window : m_permanentWindows)
    {
        window.second.Render();
    }

    for (auto window : m_oneTimeWindows)
    {
        window.Render();
    }
    m_oneTimeWindows.clear();

    const InputManager* inputs = InputManager::GetSingletonInstanceRead();
    // Display the Dear ImGui toolkit helper so we never have to look too long to know what we can get done !
    if (inputs->GetKeyState(BLA_KEY_LEFT_SHIFT).IsDown() &&
        inputs->GetKeyState(BLA_KEY_LEFT_ALT).IsDown() &&
        inputs->GetKeyState(BLA_KEY_GRAVE_ACCENT).IsRisingEdge())
    {
        g_show_demo_window = !g_show_demo_window;
    }
    if (g_show_demo_window)
    {
        ImGui::ShowDemoWindow(&g_show_demo_window);
    }

    ImGui::PopFont();

    ImGui::Render();

    int display_w, display_h;
    glfwMakeContextCurrent(m_window);
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

blaBool BlaGuiManager::IsMouseOverGui() const
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

void BlaGuiManager::DrawText(std::string textToDraw, blaIVec2 renderWindowPosition)
{
    BlaGuiWindow textWindow(std::string(""), renderWindowPosition);
    
    m_oneTimeWindows.emplace_back(BlaGuiWindow(std::string(""), renderWindowPosition));

    m_oneTimeWindows.back().SetRootElement(new BlaGuiTextElement(textToDraw));
}

void BlaGuiMenuItem::Render()
{
    ImGui::MenuItem(m_name.c_str(), NULL, m_switch);
}

void BlaGuiMenuTab::Render()
{
    if (ImGui::BeginMenu(m_name.c_str(),true))
    {
        for (int i = 0; i < m_menuItems.size(); ++i)
        {
            m_menuItems[i].Render();
        }
        ImGui::EndMenu();
    }
}

void BlaGuiMenu::Render()
{
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::PushFont(f2);
        ImGui::Text("BLAengine");
        ImGui::PopFont();
        for (int i = 0; i < m_menuTabs.size(); ++i)
        {
            m_menuTabs[i].Render();
        }

        ImGui::EndMainMenuBar();
    }
}
