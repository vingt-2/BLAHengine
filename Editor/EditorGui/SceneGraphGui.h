#pragma once

#include <Gui/GuiManager.h>
#include "Core/GameObject.h"
#include "Gui/GuiElements.h"

namespace BLAengine
{
    class SceneGraphGui
    {
    public:
        SceneGraphGui() : m_window(*BlaGuiManager::GetSingletonInstance()->OpenWindow("Scene Graph")) {}

        void UpdateSceneGraph();

    private:
        typedef blaMap<GameObjectID, BlaGuiElement*> ElementMap;
        void AddObjectToTree(ElementMap& elementMap, const GameObject& object);

        void OnGuiElementSelected(const BlaGuiElementEventPayload& event);

        static void StaticOnGuiElementSelected(void* thisPtr, const BlaGuiElementEventPayload& event) { ((SceneGraphGui*)thisPtr)->OnGuiElementSelected(event); }
        BlaGuiWindow& m_window;
        ElementMap m_elementMap;
    };
}
