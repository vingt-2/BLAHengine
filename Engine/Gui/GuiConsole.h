#pragma once

#include "Maths/Maths.h"
#include "GuiElements.h"

struct ImGuiInputTextCallbackData;
namespace BLA
{
    class BlaGuiElement;
    class Console;

    class BlaGuiConsole : public BlaGuiElement
    {
    private:
        Console* m_pConsoleSingleton;

        blaU32 m_maxLineCount;

    public:
        BlaGuiConsole(const blaString& name, Console* pConsoleSingleton) :
            BlaGuiElement(name, BlaStringId("Console")),
            m_pConsoleSingleton(pConsoleSingleton)
        {
            m_maxLineCount = 200;
        }

        void Render() override;

        static int HandleCmdCallbacks(ImGuiInputTextCallbackData* userData);
    };
}
