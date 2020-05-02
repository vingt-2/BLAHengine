#pragma once

#include "Gui/GuiManager.h"
#include "Core/GameObject.h"
#include "Gui/GuiElements.h"

namespace BLA
{
    class SceneGraphGui
    {
    public:
        SceneGraphGui() : m_selectedElement(nullptr) {}

        void UpdateSceneGraph();

        void OpenSceneGraph();

    private:
        typedef blaMap<GameObjectID, BlaGuiElement*> ElementMap;
        void AddObjectToTree(BlaGuiElement& rootElement, ElementMap& elementMap, const GameObject& object);

        void OnSceneGraphElementEvent(const BlaGuiElementEventPayload& event);

        static void StaticOnSceneGraphElementEvent(void* thisPtr, const BlaGuiElementEventPayload& event)
        {
            ((SceneGraphGui*)thisPtr)->OnSceneGraphElementEvent(event);
        }

        ElementMap m_elementMap;

        BlaGuiElement* m_selectedElement;
    };
}
