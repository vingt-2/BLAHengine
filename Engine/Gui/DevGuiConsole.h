// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"
#include "DevGuiElements.h"

struct ImGuiInputTextCallbackData;
namespace BLA
{
    class DevGuiElement;
    class Console;

    class DevGuiConsole : public DevGuiElement
    {
    private:
        Console* m_pConsoleSingleton;

        blaU32 m_maxLineCount;

    public:
        DevGuiConsole(const blaString& name, Console* pConsoleSingleton) :
            DevGuiElement(name, BlaStringId("Console")),
            m_pConsoleSingleton(pConsoleSingleton)
        {
            m_maxLineCount = 10000;
        }

        void Render() override;

        static int HandleCmdCallbacks(ImGuiInputTextCallbackData* userData);
    };
}
