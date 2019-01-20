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

#include <Common/dirent.h>

void BlaGuiManager::FileBrowser(const std::string startingDirectory, std::string& selection)
{
    enum EntryType : blaU32
    {
        DIRECTORY = 0,
        REGULAR_FILE
    } entryType;

    std::vector<std::string, EntryType> result;
    DIR *dir;
    struct dirent *ent;

    dir = opendir(startingDirectory.c_str());
    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            switch (ent->d_type)
            {
            case DT_DIR:
                result.push_back(std::pair<std::string, EntryType>(ent->d_name, DIRECTORY));
                break;
            case DT_REG:
                result.push_back(std::pair<std::string,EntryType>(ent->d_name, REGULAR_FILE));
                break;
            }
        }
    }
    closedir(dir);
}
