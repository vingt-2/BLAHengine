#pragma once

#include "Maths/Maths.h"
#include "StdInclude.h"

namespace BLA
{
    class BlaGuiElement;
    class Renderer;

    class BlaGuiWindow
    {
        friend class BlaGuiManager;
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

        BLACORE_API BlaGuiWindow() :
            m_windowName("")
            , m_windowPosition(blaIVec2(0))
            , m_windowFlags(0)
            , m_rootElement(nullptr)
            , m_bOpenWindow(true)
        {}

        BLACORE_API BlaGuiWindow(const blaString& windowName, const blaIVec2& windowPosition)
            : m_windowName(windowName)
            , m_windowPosition(windowPosition)
            , m_windowFlags(0)
            , m_rootElement(nullptr)
            , m_bOpenWindow(true)
        {};

        virtual void Render();

        BLACORE_API BlaGuiElement* RootElement() const
        {
            return m_rootElement;
        }

        BLACORE_API void SetRootElement(BlaGuiElement* imGuiElements);

        BLACORE_API blaString GetWindowName() const
        {
            return m_windowName;
        }

        BLACORE_API blaIVec2 GetWindowPosition() const
        {
            return m_windowPosition;
        }

        BLACORE_API bool HasFocus() const { return m_hasFocus; }

    protected:
        blaU32 m_windowFlags;
        blaString m_windowName;
        blaIVec2 m_windowPosition;
        BlaGuiElement* m_rootElement;
        bool m_hasFocus;
        bool m_bOpenWindow;
    };

    class BlaOneTimeWindow : public BlaGuiWindow
    {
    public:
        BLACORE_API BlaOneTimeWindow() : BlaGuiWindow()
        {}

        BLACORE_API  BlaOneTimeWindow(const blaString& windowName, const blaIVec2& windowPosition) :
            BlaGuiWindow(windowName, windowPosition)
        {};

        void Render() override;
    };

    class BlaGuiRenderWindow : public BlaGuiWindow
    {
    public:
        BLACORE_API BlaGuiRenderWindow(Renderer* renderer) : BlaGuiWindow(), m_pRenderer(renderer)
        {}

        BLACORE_API BlaGuiRenderWindow(Renderer* renderer, const blaString& windowName, const blaIVec2& windowPosition) :
            BlaGuiWindow(windowName, windowPosition), m_pRenderer(renderer)
        {};

        void Render() override;

        BLACORE_API blaVec2 GetMousePointerScreenSpaceCoordinates() const { return m_cursorScreenSpacePosition; }

    private:
        blaVec2 m_cursorScreenSpacePosition;
        Renderer* m_pRenderer;
    };
}
