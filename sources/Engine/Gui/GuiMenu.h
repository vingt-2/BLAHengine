#pragma once

#include <Common/Maths/Maths.h>

#include <string>
#include <vector>

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
        BlaGuiMenuItem(std::string name, blaBool* bool_switch) :
            m_name(name)
           ,m_switch(bool_switch)
        {}

    private:
        std::string m_name;
        blaBool* m_switch;
        void Render();
    };

    class BlaGuiMenuTab
    {
        friend class BlaGuiMenu;
    public:
        BlaGuiMenuTab(std::string name):
            m_name(name)
        {}

        void AddMenu(BlaGuiMenuItem& menu)
        {
            m_menuItems.emplace_back(menu);
        }

    private:
        std::string m_name;
        std::vector<BlaGuiMenuItem> m_menuItems;
        void Render();
    };

    struct BlaGuiMenu
    {
        std::vector<BlaGuiMenuTab> m_menuTabs;
        void Render();
    };
}
