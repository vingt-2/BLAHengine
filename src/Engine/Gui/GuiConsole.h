#pragma once

#include <Common/Maths/Maths.h>
#include "GuiElements.h"

struct ImGuiInputTextCallbackData;
namespace BLAengine
{
    class BlaGuiElement;
    class Console;

    class BlaGuiConsole : public BlaGuiElement
    {
    private:
        Console* m_pConsoleSingleton;

        blaU32 m_maxLineCount;

    public:
        BlaGuiConsole(Console* pConsoleSingleton):
            BlaGuiElement(),
            m_pConsoleSingleton(pConsoleSingleton)
        {
            m_maxLineCount = 200;
        }

        void Render() override;

        static int HandleCmdCallbacks(ImGuiInputTextCallbackData* userData);
    };
}
