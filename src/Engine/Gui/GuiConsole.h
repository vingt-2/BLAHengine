#pragma once

#include <Common/Maths/Maths.h>
#include "GuiElements.h"

namespace BLAengine
{
    class BlaGuiElement;
    class Console;

    class BlaGuiConsole : public BlaGuiElement
    {
    private:
        Console* m_pConsoleSingleton;
    public:
        BlaGuiConsole(Console* pConsoleSingleton):
            BlaGuiElement(),
            m_pConsoleSingleton(pConsoleSingleton)
        {}

        void Render() override;
    };
}
