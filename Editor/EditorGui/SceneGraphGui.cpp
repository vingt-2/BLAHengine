// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "SceneGraphGui.h"

#include "Gui/DevGuiManager.h"
#include "Gui/DevGuiElements.h"
#include "EditorCommands.h"
#include "Core/Scene.h"
#include "EngineInstance.h"

using namespace BLA;

#define SCENE_GRAPH_ELEMENT_GROUP_ID BlaStringId("SceneGraphElement")

void SceneGraphGui::AddObjectToTree(DevGuiElement& rootElement, ElementMap& elementMap, const Core::GameObject& object)
{
    ElementMap::iterator it = elementMap.find(object);

    if (it != elementMap.end())
    {
        return;
    }

    DevGuiElement* element = new DevGuiCollapsibleElement(blaString(object.GetId()), SCENE_GRAPH_ELEMENT_GROUP_ID);
    DevGuiRegisteredEvents eventListener = 
    {
        DevGuiElementEventPayload::EventType::SELECTED | 
        DevGuiElementEventPayload::EventType::DOUBLE_CLICKED |
        DevGuiElementEventPayload::EventType::ELEMENT_DROPPED
        , StaticOnSceneGraphElementEvent, this
    };

    element->RegisterEvents(eventListener);

    if (object.GetParent().IsValid())
    {
        ElementMap::iterator parentIt = elementMap.find(object.GetParent().GetId());

        if (parentIt == elementMap.end())
        {
            AddObjectToTree(rootElement, elementMap, object.GetParent());
        }
        parentIt = elementMap.find(object.GetParent().GetId());
        rootElement.AddChildAfterNode(element, parentIt->second);
    }
    else
    {
        rootElement.AddChild(element);
    }

    elementMap.insert(blaPair<Core::GameObjectID, DevGuiElement*>(object, element));
}

// TODO: Huh... Really, I'm referencing a console command here ? Change that ... ?
extern void SelectObject(blaString name);
extern void FocusOnObject(blaString name);
extern void SetParent(blaString parent, blaString child);
void SceneGraphGui::OnSceneGraphElementEvent(const DevGuiElementEventPayload& event)
{
    switch(event.m_eventType)
    {
        case DevGuiElementEventPayload::EventType::SELECTED:
        {
            DevGuiElement* selected = (DevGuiElement*)event.m_pEventPayload;
            SelectObject(selected->GetName());
            if (m_selectedElement)
                m_selectedElement->m_isSelected = false;
            m_selectedElement = selected;
            m_selectedElement->m_isSelected = true;
        }
        break;
        case DevGuiElementEventPayload::EventType::DOUBLE_CLICKED:
        {
            DevGuiElement* selected = (DevGuiElement*)event.m_pEventPayload; 
            FocusOnObject(selected->GetName());
        }
        break;
        case DevGuiElementEventPayload::EventType::ELEMENT_DROPPED:
        {
            BlaDroppablePayload* droppedPayload = (BlaDroppablePayload*)event.m_pEventPayload;
            SetParent(droppedPayload->m_receiverElement->GetName(), droppedPayload->m_droppedElement->GetName());
        }
        default:
        break;
    }
}

SceneGraphGui::SceneGraphGui(EditorCommandManager* m_commandManager): m_selectedElement(nullptr)
{
}

void SceneGraphGui::UpdateSceneGraph()
{
    DevGuiWindow* window = DevGuiManager::GetSingletonInstance()->GetWindow("Scene Graph");

    if (!window) 
        return;

    Core::Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();

    delete window->RootElement();

    DevGuiElement* root = new DevGuiSimpleTextElement("SceneRoot", SCENE_GRAPH_ELEMENT_GROUP_ID, "Scene:");
    DevGuiRegisteredEvents eventListener =
    {
        DevGuiElementEventPayload::EventType::ELEMENT_DROPPED
        , StaticOnSceneGraphElementEvent, this
    };
    root->RegisterEvents(eventListener);

    window->SetRootElement(root);

    m_elementMap.clear();

    for (const Core::GameObject& object : scene->GetObjects())
    {
        AddObjectToTree(*window->RootElement(), m_elementMap, object);
    }
}

void SceneGraphGui::OpenSceneGraph()
{
    DevGuiManager::GetSingletonInstance()->OpenWindow("Scene Graph");
    UpdateSceneGraph();
}
