#pragma once

#include "Maths/Maths.h"

#include "StdInclude.h"

namespace BLA
{
    class BlaGuiElement;

    struct MenuCallback
    {
        void* m_objectPtr;
        void(*m_functionPtr)(void*);
    };

    class BlaGuiMenuBase
    {
        friend class BlaGuiMenuTab;
        friend class BlaGuiMenu;
        virtual void Render() = 0;

    protected:
        virtual ~BlaGuiMenuBase() {};
    };

    class BlaGuiMenuItem : public BlaGuiMenuBase
    {
        friend class BlaGuiMenuTab;
        friend class BlaGuiMenu;
    	
        BlaGuiMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator = false);

        void Render() override;

        blaString m_name;
        blaBool* m_switch;

        blaBool m_endWithSeparator;
    };

    class BlaGuiMenuTab : public BlaGuiMenuBase
    {
        friend class BlaGuiMenu;
    	
        BlaGuiMenuTab(blaString name);
    	virtual ~BlaGuiMenuTab();
    	
        blaString m_name;
        blaVector<BlaGuiMenuBase*> m_menuItems;
        void Render() override;

    public:
    	
        BLACORE_API void AddMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator = false);

        BLACORE_API BlaGuiMenuTab& AddSubMenu(blaString name);
    };

    class BlaGuiMenu
    {
        friend class BlaGuiManager;
        friend class BlaGuiWindow;
        blaVector<BlaGuiMenuBase*> m_menuTabs;
        void Render();

    public:

        ~BlaGuiMenu();
    	
        BLACORE_API void AddMenuItem(blaString name, blaBool* bool_switch, blaBool endWithSeparator = false);

        BLACORE_API BlaGuiMenuTab& AddSubMenu(blaString name);
    };
}
