#pragma once

#include "Engine/Core/GameObject.h"
#include "Engine/Gui/GuiManager.h"

namespace BLAengine
{
    class GameObjectInspector
    {
        GameObjectReference m_selectedGameObject;

        BlaGuiWindow& m_window;

    public:
        GameObjectInspector() : m_window(*BlaGuiManager::GetSingletonInstance()->OpenWindow("GameObject Inspector")) {}

        void InspectGameObject(GameObjectReference gameObject);

        static blaVector<blaString> GetComponents(GameObjectReference gameObject);
    };
}
