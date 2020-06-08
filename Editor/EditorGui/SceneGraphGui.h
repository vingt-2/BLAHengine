// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/GameObject.h"

namespace BLA
{
    class EditorCommandManager;
    class DevGuiElement;
    struct DevGuiElementEventPayload;
    class SceneGraphGui
    {
    public:
        SceneGraphGui(EditorCommandManager* m_commandManager);

        void UpdateSceneGraph();

        void OpenSceneGraph();

    private:
        typedef blaMap<GameObjectID, DevGuiElement*> ElementMap;
        void AddObjectToTree(DevGuiElement& rootElement, ElementMap& elementMap, const GameObject& object);

        void OnSceneGraphElementEvent(const DevGuiElementEventPayload& event);

        static void StaticOnSceneGraphElementEvent(void* thisPtr, const DevGuiElementEventPayload& event)
        {
            ((SceneGraphGui*)thisPtr)->OnSceneGraphElementEvent(event);
        }

        ElementMap m_elementMap;

        DevGuiElement* m_selectedElement;
    };
}
