#pragma once

#include <Common/Maths/Maths.h>

namespace BLAengine
{
    class BlaGuiElement;

    class BlaGuiWindow
    {
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

        BlaGuiWindow(const std::string& windowName, const blaIVec2& windowPosition)
            : m_windowName(windowName),
            m_windowPosition(windowPosition)
        {};

        void Render();

        void ShowTitleBar(blaBool set);

    private:

        blaU32 m_windowFlags = 0;

        BlaGuiElement* m_rootElement;

    public:
        BlaGuiElement* RootElement() const
        {
            return m_rootElement;
        }

        void SetRootElement(BlaGuiElement* imGuiElements)
        {
            m_rootElement = imGuiElements;
        }

        std::string WindowName() const
        {
            return m_windowName;
        }

        blaIVec2 WindowPosition() const
        {
            return m_windowPosition;
        }

    private:
        std::string m_windowName;
        blaIVec2 m_windowPosition;
    };
}
