// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"
#include "StdInclude.h"

namespace BLA
{
    class VulkanRenderer;
    class DevGuiElement;
    class DevGuiMenu;
    class Renderer;

    class DevGuiWindow
    {
        friend class DevGuiManager;
        
    public:
        enum WindowFlags
        {
            None = 0,
            NoTitleBar = 1 << 0,
            NoResize = 1 << 1,
            NoMove = 1 << 2,
            NoScrollbar = 1 << 3,
            NoScrollWithMouse = 1 << 4,
            NoCollapse = 1 << 5,
            AlwaysAutoResize = 1 << 6,
            NoBackground = 1 << 7,
            NoSavedSettings = 1 << 8,
            NoMouseInputs = 1 << 9,
            MenuBar = 1 << 10,
            HorizontalScrollbar = 1 << 11,
            NoFocusOnAppearing = 1 << 12,
            NoBringToFrontOnFocus = 1 << 13,
            AlwaysVerticalScrollbar = 1 << 14,
            AlwaysHorizontalScrollbar = 1 << 15,
            AlwaysUseWindowPadding = 1 << 16,
            NoNavInputs = 1 << 18,
            NoNavFocus = 1 << 19,
            UnsavedDocument = 1 << 20,
            NoDocking = 1 << 21,
        };

        virtual void Render();

        BLACORE_API DevGuiElement* RootElement() const;

        BLACORE_API void SetRootElement(DevGuiElement* imGuiElements);

        BLACORE_API blaString GetWindowName() const;

        BLACORE_API blaIVec2 GetWindowPosition() const;

        BLACORE_API bool HasFocus() const;

        BLACORE_API DevGuiMenu& AddMenu();

    protected:

        DevGuiWindow();
        DevGuiWindow(const blaString& windowName, const blaIVec2& windowPosition);
        ~DevGuiWindow();
        
        blaU32 m_windowFlags;
        blaString m_windowName;
        blaIVec2 m_windowPosition;
        DevGuiElement* m_rootElement;
        DevGuiMenu* m_menu;
        bool m_hasFocus;
        bool m_bOpenWindow;
    };

    inline blaIVec2 DevGuiWindow::GetWindowPosition() const
    {
        return m_windowPosition;
    }

    inline blaString DevGuiWindow::GetWindowName() const
    {
        return m_windowName;
    }

    class BlaOneTimeWindow : public DevGuiWindow
    {
    public:
        BLACORE_API BlaOneTimeWindow();

        BLACORE_API BlaOneTimeWindow(const blaString& windowName, const blaIVec2& windowPosition);

        void Render() override;
    };

    struct RenderWindowData;
    class DevGuiRenderViewportWindow : public DevGuiWindow
    {
    public:
        BLACORE_API DevGuiRenderViewportWindow(VulkanRenderer* renderer, const blaString& windowName, const blaIVec2& windowPosition);

        void Render() override;

        BLACORE_API blaVec2 GetMousePointerScreenSpaceCoordinates() const;

    private:

        void UpdateDisplayTexture(VulkanRenderer* renderer);

        blaVec2 m_cursorScreenSpacePosition;
        VulkanRenderer* m_pRenderer;

        RenderWindowData* m_renderData;
    };
}