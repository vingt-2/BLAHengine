#pragma once

#include "Core/GameObject.h"

namespace BLA
{
    class EditorCommandManager;
    class BlaGuiElement;
    struct BlaGuiElementEventPayload;
    class SceneGraphGui
    {
    public:
        SceneGraphGui(EditorCommandManager* m_commandManager);

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
