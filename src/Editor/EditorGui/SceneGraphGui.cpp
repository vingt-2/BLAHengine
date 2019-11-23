#include "SceneGraphGui.h"

#include <Engine/Core/Scene.h>
#include <Engine/EngineInstance.h>

using namespace BLAengine;

void SceneGraphGui::AddObjectToTree(ElementMap& elementMap, const GameObjectReference& object)
{
    auto root = m_window.RootElement();

    ElementMap::iterator it = elementMap.find(object->GetName());

    if (it != elementMap.end())
    {
        return;
    }

    BlaGuiElement* element = new BlaGuiCollapsibleElement(object->GetName());
    BlaGuiRegisteredEvents eventListener = { BlaGuiElementEventPayload::EventType::SELECTED, StaticOnGuiElementSelected, this };

    element->RegisterEvents(eventListener);

    if (object->GetParent().IsValid())
    {
        ElementMap::iterator parentIt = elementMap.find(object->GetParent()->GetName());

        if (parentIt == elementMap.end())
        {
            AddObjectToTree(elementMap, object->GetParent());
        }
        parentIt = elementMap.find(object->GetParent()->GetName());
        root->AddChildAfterNode(element, parentIt->second);
    }
    else
    {
        root->AddChild(element);
    }

    elementMap.insert(blaPair<blaString, BlaGuiElement*>(object->GetName(), element));
}

extern int SelectObject(blaString name);
void SceneGraphGui::OnGuiElementSelected(const BlaGuiElementEventPayload& event)
{
    if (event.m_eventType == BlaGuiElementEventPayload::EventType::SELECTED)
    {
        SelectObject(event.m_pGuiElement->GetName());
    }
}

void SceneGraphGui::UpdateSceneGraph()
{
    Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();

    delete m_window.RootElement();

    BlaGuiElement* root = new BlaGuiSimpleTextElement("SceneRoot", "");

    m_window.SetRootElement(root);

    ElementMap elementMap;

    for (const GameObjectReference& object : scene->GetObjects())
    {
        AddObjectToTree(elementMap, object);
    }
}
