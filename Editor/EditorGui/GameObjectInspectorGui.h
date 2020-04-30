#pragma once

#include "Core/GameObject.h"
#include "Gui/GuiManager.h"

namespace BLA
{
    class GameObjectInspector
    {
        GameObject m_selectedGameObject;

        BlaGuiWindow& m_window;

    public:
        GameObjectInspector() : m_window(*BlaGuiManager::GetSingletonInstance()->OpenWindow("GameObject Inspector")) {}

        void InspectGameObject(GameObject gameObject);

        static blaVector<blaString> GetComponents(GameObject gameObject);
    };
}
