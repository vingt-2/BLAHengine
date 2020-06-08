// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"

#include "StdInclude.h"

namespace BLA
{
    class DevGuiElement;

    struct MenuCallback
    {
        void* m_objectPtr;
        void(*m_functionPtr)(void*);
    };

    class DevGuiMenuBase
    {
        friend class DevGuiMenuTab;
        friend class DevGuiMenu;
        virtual void Render() = 0;

    protected:
        virtual ~DevGuiMenuBase() {};
    };

    class DevGuiMenuItem : public DevGuiMenuBase
    {
        friend class DevGuiMenuTab;
        friend class DevGuiMenu;
    	
        DevGuiMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator = false);

        void Render() override;

        blaString m_name;
        blaBool* m_switch;

        blaBool m_endWithSeparator;
    };

    class DevGuiMenuTab : public DevGuiMenuBase
    {
        friend class DevGuiMenu;
    	
        DevGuiMenuTab(blaString name);
    	virtual ~DevGuiMenuTab();
    	
        blaString m_name;
        blaVector<DevGuiMenuBase*> m_menuItems;
        void Render() override;

    public:
    	
        BLACORE_API void AddMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator = false);

        BLACORE_API DevGuiMenuTab& AddSubMenu(blaString name);
    };

    class DevGuiMenu
    {
        friend class DevGuiManager;
        friend class DevGuiWindow;
        blaVector<DevGuiMenuBase*> m_menuTabs;
        void Render();

    public:

        ~DevGuiMenu();
    	
        BLACORE_API void AddMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator = false);

        BLACORE_API DevGuiMenuTab& AddSubMenu(blaString name);
    };
}
