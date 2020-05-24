#pragma once

#include "Core/GameObject.h"

namespace BLA
{
    class BlaGuiWindow;
    class EditorCommandManager;
    class GameObjectInspector
    {
        GameObject m_selectedGameObject;
        EditorCommandManager* m_editorCommandManager;
        BlaGuiWindow& m_window;

    public:
        GameObjectInspector(EditorCommandManager* editorCommandManager);

        void InspectGameObject(GameObject gameObject);

        static blaVector<blaString> GetComponents(GameObject gameObject);
    };
}
