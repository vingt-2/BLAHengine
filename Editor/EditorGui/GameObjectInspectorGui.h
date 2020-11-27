// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/GameObject.h"

namespace BLA
{
    class DevGuiWindow;
    class EditorCommandManager;
    class GameObjectInspector
    {
        Core::GameObject m_selectedGameObject;
        EditorCommandManager* m_editorCommandManager;
        DevGuiWindow& m_window;

    public:
        GameObjectInspector(EditorCommandManager* editorCommandManager);

        void InspectGameObject(Core::GameObject gameObject);

        static blaVector<blaString> GetComponents(Core::GameObject gameObject);
    };
}
