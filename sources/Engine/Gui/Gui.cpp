#include "GuiManager.h"
#include "GuiWindow.h"
#include "GuiElements.h"

#include <External/imgui/imgui.h>
#include <External/imgui/examples/imgui_impl_glfw.h>
#include <External/imgui/examples/imgui_impl_opengl3.h>

#include <Engine/System/RenderWindow.h>
#include <Engine/System/InputManager.h>

using namespace BLAengine;
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

	f = io.Fonts->AddFontFromFileTTF("./resources/fonts/roboto-light.ttf", 22.0f);
	f2 = io.Fonts->AddFontFromFileTTF("./resources/fonts/roboto-thin.ttf", 24.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
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
    
    m_rootElement->Render();

    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::End();
    // END OCornut's Dear ImGui Specific Code Now
}

void BlaGuiWindow::ShowTitleBar(blaBool set)
{
    if (set)
        m_windowFlags &= ~(ImGuiWindowFlags_NoTitleBar);
    else
        m_windowFlags |= ImGuiWindowFlags_NoTitleBar;
}

bool g_show_demo_window = false;

void BlaGuiManager::Update()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	ImGui::PushFont(f); // PopFont()


    for (auto window : m_imguiWindows)
    {
        window->Render();
    }
    m_imguiWindows.clear();

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

    bool bla;
	ImGui::PushFont(f2);
	if (ImGui::BeginMainMenuBar())
    {
		ImGui::Text("BLAengine");
		ImGui::PopFont();
		ImGui::PushFont(f);
        if (ImGui::BeginMenu("Menu"))
        {
            ImGui::MenuItem("Main menu bar", NULL, &bla);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples"))
        {
            ImGui::MenuItem("Main menu bar", NULL, &bla);
            ImGui::MenuItem("Console", NULL, &bla);
            ImGui::MenuItem("Log", NULL, &bla);
            ImGui::MenuItem("Simple layout", NULL, &bla);
            ImGui::MenuItem("Property editor", NULL, &bla);
            ImGui::MenuItem("Long text display", NULL, &bla);
            ImGui::MenuItem("Auto-resizing window", NULL, &bla);
            ImGui::MenuItem("Constrained-resizing window", NULL, &bla);
            ImGui::MenuItem("Simple overlay", NULL, &bla);
            ImGui::MenuItem("Manipulating window titles", NULL, &bla);
            ImGui::MenuItem("Custom rendering", NULL, &bla);
            ImGui::MenuItem("Dockspace", NULL, &bla);
            ImGui::MenuItem("Documents", NULL, &bla);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Metrics", NULL, &bla);
            ImGui::MenuItem("Style Editor", NULL, &bla);
            ImGui::MenuItem("About Dear ImGui", NULL, &bla);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

	ImGui::PopFont();

    ImGui::Render();
    int display_w, display_h;
    glfwMakeContextCurrent(m_window);
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void BlaGuiManager::DrawText(std::string textToDraw, blaIVec2 renderWindowPosition)
{
    BlaGuiWindow* textWindow = new BlaGuiWindow(std::string(""), renderWindowPosition);
    
    textWindow->ShowTitleBar(false);

    BlaGuiElement* textElement = new BlaGuiTextElement(textToDraw);

    textWindow->SetRootElement(textElement);

    m_imguiWindows.push_back(textWindow);
}
