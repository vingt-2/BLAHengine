#include "GuiManager.h"
#include "GuiWindow.h"
#include "GuiElements.h"
#include "GuiMenu.h"
#include "GuiConsole.h"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "System/RenderWindow.h"
#include "System/InputManager.h"
#include "System/FileSystem/Files.h"
#include "System/Console.h"
#include "Core/Scene.h"
#include "Renderer/GL33Renderer.h"

#include <iomanip>
#include "EngineInstance.h"

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(BlaGuiManager);

blaBool IsItemDoubleCliked(int mouse_button = 0);

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

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 18.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 12.0f);
}

ImFont* f;
ImFont* f2;

BlaGuiWindow* BlaGuiManager::OpenWindow(blaString name)
{
    m_openWindows.insert(blaPair<blaString, BlaGuiWindow*>(name, new BlaGuiWindow(name, blaIVec2(10, 10))));
    return m_openWindows[name];
}

void BlaGuiManager::OpenWindow(blaString name, BlaGuiWindow* window)
{
    m_openWindows.insert(blaPair<blaString, BlaGuiWindow*>(name, window));
}

void BlaGuiManager::Init()
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
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

}

void BlaGuiManager::Destroy()
{
    for (auto window : m_openWindows)
    {
        delete window.second;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void BlaGuiElement::SendEvent(BlaGuiElementEventPayload::EventType eventType)
{
    for (const auto& cb : m_registeredCallbacks)
    {
        if (cb.m_eventTriggerFlags & eventType != 0)
        {
            BlaGuiElementEventPayload callbackPayload{ eventType, this };
            cb.m_callback(cb.m_callerPtr, callbackPayload);
        }
    }
}

void BlaGuiElement::RegisterEvents(BlaGuiRegisteredEvents& cb)
{
    blaVector<BlaGuiRegisteredEvents>::iterator it = std::find_if(m_registeredCallbacks.begin(), m_registeredCallbacks.end(),
        [cb](const BlaGuiRegisteredEvents& c) {return cb.m_callback == c.m_callback; });

    if (it == m_registeredCallbacks.end())
    {
        m_registeredCallbacks.push_back(cb);
    }
}

void BlaGuiElement::UnRegisterEvents(BlaGuiRegisteredEvents& cb)
{
    blaVector<BlaGuiRegisteredEvents>::iterator it = std::find_if(m_registeredCallbacks.begin(), m_registeredCallbacks.end(),
        [cb](const BlaGuiRegisteredEvents& c) {return cb.m_callback == c.m_callback; });

    if (it != m_registeredCallbacks.end())
    {
        m_registeredCallbacks.erase(it);
    }
}

void BlaGuiElement::Render()
{
    auto child = GetChild();

    while (child != nullptr)
    {
        child->Render();
        child = child->GetNext();
    }
}

void BlaGuiCollapsibleElement::Render()
{
    int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (!m_child)
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (ImGui::TreeNodeEx((void*)(intptr_t)this, flags, GetName().c_str()))
    {
        if (ImGui::IsItemClicked())
        {
            SendEvent(BlaGuiElementEventPayload::EventType::SELECTED);
        }

        auto child = GetChild();

        while (child != nullptr)
        {
            child->Render();
            child = child->GetNext();
        }

        ImGui::TreePop();
    }
}

void BlaGuiCollapsibleHeaderElement::Render()
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

void BlaGuiSimpleTextElement::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::Text(m_text.c_str());
    // END OCornut's Dear ImGui Specific Code Now

    BlaGuiElement::Render();
}

template<>
void BlaGuiEditElement<bool>::Render()
{
    ImGui::Checkbox(GetName().c_str(), m_pToValue);
}

template<>
void BlaGuiEditElement<blaF32>::Render()
{
    ImGui::InputFloat(GetName().c_str(), m_pToValue, 0.1f, 1.f, 7);
}

template<>
void BlaGuiEditElement<blaF64>::Render()
{
    ImGui::InputDouble(GetName().c_str(), m_pToValue, 0.1, 1.0);
}

template<>
void BlaGuiEditElement<blaS32>::Render()
{
    ImGui::InputInt(GetName().c_str(), m_pToValue, 1, 10);
}

template<>
void BlaGuiEditElement<blaVec2>::Render()
{
    ImGui::InputFloat2(GetName().c_str(), &(m_pToValue->x));
}

template<>
void BlaGuiEditElement<blaVec3>::Render()
{
    ImGui::InputFloat3(GetName().c_str(), &(m_pToValue->x));
}

template<>
void BlaGuiEditElement<blaQuat>::Render()
{
    ImGui::InputFloat4(GetName().c_str(), &(m_pToValue->x));
}

template<>
void BlaGuiEditElement<blaPosQuat>::Render()
{
    ImGui::InputFloat3((GetName() + " P").c_str(), &(m_pToValue->GetTranslation().x));
    ImGui::InputFloat4((GetName() + " R").c_str(), &(m_pToValue->GetRotation().x));
}

template<>
void BlaGuiEditElement<blaScaledTransform>::Render()
{
    ImGui::InputFloat3((GetName() + " P").c_str(), &(m_pToValue->m_posQuat.GetTranslation().x));
    ImGui::InputFloat4((GetName() + " R").c_str(), &(m_pToValue->m_posQuat.GetRotation().x));
    ImGui::InputFloat3((GetName() + " S").c_str(), &(m_pToValue->m_scale.x));
}

template<>
void BlaGuiEditElement<blaString>::Render()
{
    char inputBuf[2048];
    strcpy(inputBuf, m_pToValue->c_str());
    ImGui::InputText(GetName().c_str(), inputBuf, sizeof(inputBuf));
    *m_pToValue = blaString(inputBuf);
}

template <typename T>
void BlaGuiEditElementVector<T>::Render()
{
    for (int i = 0; i < m_pToVector->size(); i++)
    {
        BlaGuiEditElement<T> toRender = BlaGuiEditElement<T>(GetName() + std::to_string(i), (&m_pToVector[0]) + i * sizeof(T));
        toRender.Render();
    }
}

template<>
void BlaGuiEditElement<GameObject>::Render()
{
    ImGui::Columns(2);
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
    ImGui::Columns();
}

void BlaGuiWindow::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImVec2 position(m_windowPosition.x, m_windowPosition.y);
    ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
    ImGui::Begin(m_windowName.c_str(), &m_bOpenWindow, m_windowFlags);
    // END OCornut's Dear ImGui Specific Code Now

    if (m_rootElement)
    {
        m_rootElement->Render();
    }

    m_hasFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_None);
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::End();
    // END OCornut's Dear ImGui Specific Code Now
}

void BlaGuiWindow::SetRootElement(BlaGuiElement* imGuiElements)
{
    m_rootElement = imGuiElements;
}

void BlaOneTimeWindow::Render()
{
    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImVec2 position(m_windowPosition.x, m_windowPosition.y);
    ImGui::SetNextWindowPos(position);
    ImGui::Begin(m_windowName.c_str(), NULL, m_windowFlags | BlaGuiWindow::WindowFlags::MenuBar | ImGuiWindowFlags_NoTitleBar);
    // END OCornut's Dear ImGui Specific Code Now

    if (m_rootElement)
    {
        m_rootElement->Render();
    }

    // BEGIN OCornut's Dear ImGui Specific Code Now
    ImGui::End();
    // END OCornut's Dear ImGui Specific Code Now
}

void BlaGuiRenderWindow::Render()
{
    if (GL33Renderer* renderer = dynamic_cast<GL33Renderer*>(m_pRenderer))
    {
        // BEGIN OCornut's Dear ImGui Specific Code Now
        ImVec2 position(m_windowPosition.x, m_windowPosition.y);
        ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
        ImGui::Begin(m_windowName.c_str(), &m_bOpenWindow, m_windowFlags);

        ImVec2 windowPos = ImGui::GetWindowPos();

        ImGui::GetWindowDrawList()->AddImage(
            (void*)renderer->GetDisplayBufferTexture(),
            windowPos,
            ImVec2(windowPos.x + ImGui::GetWindowWidth(), windowPos.y + ImGui::GetWindowHeight()),
            ImVec2(0, 1), ImVec2(1, 0));

        ImVec2 cursorInWindow = ImGui::GetCursorPos();

        m_pRenderer->SetRenderSize(blaIVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));

        ImVec2 mouse = ImGui::GetMousePos();

        m_cursorScreenSpacePosition = blaVec2(
            1.0f - (mouse.x - windowPos.x) / ImGui::GetWindowWidth(),
            1.0f - (mouse.y - windowPos.y) / ImGui::GetWindowHeight());

        m_hasFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_None);

        ImGui::End();
    }
}

bool g_show_demo_window = false;
bool g_debugFileBrowser = false;

void BlaGuiManager::Update()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::NewFrame();

    if (m_showDockspace)
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
        if (window.second->ShouldClose())
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

    for (auto& window : m_oneTimeWindows)
    {
        window.Render();
    }
    m_oneTimeWindows.clear();

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

    int display_w, display_h;
    glfwMakeContextCurrent(m_window);
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

blaBool BlaGuiManager::IsMouseOverGui() const
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

void BlaGuiManager::DrawText(const blaString& textToDraw, blaIVec2 renderWindowPosition)
{
    m_oneTimeWindows.emplace_back(BlaOneTimeWindow(blaString(""), renderWindowPosition));

    m_oneTimeWindows.back().SetRootElement(new BlaGuiSimpleTextElement("", textToDraw));
}

void BlaGuiManager::OpenConsole(const blaString& consoleName)
{
    m_openWindows.insert(blaPair<blaString, BlaGuiWindow*>(consoleName, new BlaGuiWindow(consoleName, blaIVec2(10, 10))));
    m_openWindows[consoleName]->SetRootElement(new BlaGuiConsole(consoleName, Console::GetSingletonInstance()));
}

OpenFilePrompt* BlaGuiManager::CreateOpenFilePrompt(blaString browserName, blaBool disableMultipleSelection)
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

SaveFilePrompt* BlaGuiManager::CreateSaveFilePrompt(blaString browserName)
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


blaBool BlaGuiManager::CloseFileBrowser(blaString browserName)
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

void BlaGuiMenuItem::Render()
{
    ImGui::MenuItem(m_name.c_str(), NULL, m_switch);

    if (m_endWithSeparator)
    {
        ImGui::Separator();
    }
}

void BlaGuiMenuTab::Render()
{
    if (ImGui::BeginMenu(m_name.c_str(), true))
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
    strcpy(readonlyTextCpy, m_currentFilesDirectory.c_str());
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

void OpenFilePrompt::Render()
{
    BlaFileBrowser::Render();

    char readonlyTextCpy[500];

    blaString selectedFiles = "";
    for (auto& file : m_currentSelection)
    {
        selectedFiles += "\"" + file.first + "\" ";
    }

    strcpy(readonlyTextCpy, selectedFiles.c_str());
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

void SaveFilePrompt::Render()
{
    BlaFileBrowser::Render();

    char saveFileTextInput[500];

    blaString selectedFiles = "";
    for (auto& file : m_currentSelection)
    {
        selectedFiles += file.first;
    }

    strcpy(saveFileTextInput, selectedFiles.c_str());
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

void BlaGuiConsole::Render()
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
            strcpy(childName, (blaString("CL:") + std::to_string(i)).data());
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
        &BlaGuiConsole::HandleCmdCallbacks,
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

int BlaGuiConsole::HandleCmdCallbacks(ImGuiInputTextCallbackData* data)
{
    Console* pConsole = ((BlaGuiConsole*)(data->UserData))->m_pConsoleSingleton;
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

// You may think I'm a monster for doing this.
// And perhaps you are right

#undef IMGUI_IMPL_OPENGL_LOADER_GL3W
#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include "imgui.cpp"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "examples/imgui_impl_opengl3.cpp"
#include "examples/imgui_impl_glfw.cpp"