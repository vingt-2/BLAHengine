#pragma once

#include <Common/Maths/Maths.h>

#include <Common/StdInclude.h>

namespace BLAengine
{
    class BlaGuiElement;

    struct MenuCallback
    {
        void* m_objectPtr;
        void (*m_functionPtr)(void*);
    };

    class BlaGuiMenuItem
    {
        friend class BlaGuiMenuTab;
    public:
        BlaGuiMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator = false) :
            m_name(name)
           ,m_switch(bool_switch)
           ,m_endWithSeparator(endWithSeparator)
        {}

    private:
        void Render();
        
        blaString m_name;
        blaBool* m_switch;

        blaBool m_endWithSeparator;
    };

    class BlaGuiMenuTab
    {
        friend class BlaGuiMenu;
    public:
        BlaGuiMenuTab(blaString name):
            m_name(name)
        {}

        void AddMenu(BlaGuiMenuItem& menu)
        {
            m_menuItems.emplace_back(menu);
        }

    private:
        blaString m_name;
        blaVector<BlaGuiMenuItem> m_menuItems;
        void Render();
    };

    struct BlaGuiMenu
    {
        blaVector<BlaGuiMenuTab> m_menuTabs;
        void Render();
    };
}
