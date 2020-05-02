#include "SceneGraphGui.h"

#include "Core/Scene.h"
#include "EngineInstance.h"

using namespace BLA;

#define SCENE_GRAPH_ELEMENT_GROUP_ID BlaStringId("SceneGraphElement")

void SceneGraphGui::AddObjectToTree(BlaGuiElement& rootElement, ElementMap& elementMap, const GameObject& object)
{
    ElementMap::iterator it = elementMap.find(object);

    if (it != elementMap.end())
    {
        return;
    }

    BlaGuiElement* element = new BlaGuiCollapsibleElement(blaString(object.GetId()), SCENE_GRAPH_ELEMENT_GROUP_ID);
    BlaGuiRegisteredEvents eventListener = 
    {
        BlaGuiElementEventPayload::EventType::SELECTED | 
        BlaGuiElementEventPayload::EventType::DOUBLE_CLICKED |
        BlaGuiElementEventPayload::EventType::ELEMENT_DROPPED
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

    elementMap.insert(blaPair<GameObjectID, BlaGuiElement*>(object, element));
}

// TODO: Huh... Really, I'm referencing a console command here ? Change that ... ?
extern void SelectObject(blaString name);
extern void FocusOnObject(blaString name);
extern void SetParent(blaString parent, blaString child);
void SceneGraphGui::OnSceneGraphElementEvent(const BlaGuiElementEventPayload& event)
{
    switch(event.m_eventType)
    {
        case BlaGuiElementEventPayload::EventType::SELECTED:
        {
            BlaGuiElement* selected = (BlaGuiElement*)event.m_pEventPayload;
            SelectObject(selected->GetName());
            if (m_selectedElement)
                m_selectedElement->m_isSelected = false;
            m_selectedElement = selected;
            m_selectedElement->m_isSelected = true;
        }
        break;
        case BlaGuiElementEventPayload::EventType::DOUBLE_CLICKED:
        {
            BlaGuiElement* selected = (BlaGuiElement*)event.m_pEventPayload; 
            FocusOnObject(selected->GetName());
        }
        case BlaGuiElementEventPayload::EventType::ELEMENT_DROPPED:
        {
            BlaDroppablePayload* droppedPayload = (BlaDroppablePayload*)event.m_pEventPayload;
            SetParent(droppedPayload->m_receiverElement->GetName(), droppedPayload->m_droppedElement->GetName());
        }
        default:
        break;
    }
}

void SceneGraphGui::UpdateSceneGraph()
{
    BlaGuiWindow* window = BlaGuiManager::GetSingletonInstance()->GetWindow("Scene Graph");

    if (!window) 
        return;

    Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();

    delete window->RootElement();

    BlaGuiElement* root = new BlaGuiSimpleTextElement("SceneRoot", SCENE_GRAPH_ELEMENT_GROUP_ID, "Scene:");
    BlaGuiRegisteredEvents eventListener =
    {
        BlaGuiElementEventPayload::EventType::ELEMENT_DROPPED
        , StaticOnSceneGraphElementEvent, this
    };
    root->RegisterEvents(eventListener);

    window->SetRootElement(root);

    m_elementMap.clear();

    for (const GameObject& object : scene->GetObjects())
    {
        AddObjectToTree(*window->RootElement(), m_elementMap, object);
    }
}

void SceneGraphGui::OpenSceneGraph()
{
    BlaGuiManager::GetSingletonInstance()->OpenWindow("Scene Graph");
    UpdateSceneGraph();
}
