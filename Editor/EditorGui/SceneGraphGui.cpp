#include "SceneGraphGui.h"

#include <Core/Scene.h>
#include <EngineInstance.h>

using namespace BLAengine;

void SceneGraphGui::AddObjectToTree(ElementMap& elementMap, const GameObject& object)
{
    auto root = m_window.RootElement();

    ElementMap::iterator it = elementMap.find(object);

    if (it != elementMap.end())
    {
        return;
    }

    BlaGuiElement* element = new BlaGuiCollapsibleElement(blaString(object.GetId()));
    BlaGuiRegisteredEvents eventListener = { BlaGuiElementEventPayload::EventType::SELECTED, StaticOnGuiElementSelected, this };

    element->RegisterEvents(eventListener);

    if (object.GetParent().IsValid())
    {
        ElementMap::iterator parentIt = elementMap.find(object.GetParent().GetId());

        if (parentIt == elementMap.end())
        {
            AddObjectToTree(elementMap, object.GetParent());
        }
        parentIt = elementMap.find(object.GetParent().GetId());
        root->AddChildAfterNode(element, parentIt->second);
    }
    else
    {
        root->AddChild(element);
    }

    elementMap.insert(blaPair<GameObjectID, BlaGuiElement*>(object, element));
}

// TODO: Huh... Really, I'm referencing a console command here ? Change that ... ? no ? Actual !
extern void SelectObject(blaString name);
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

    m_elementMap.clear();

    for (const GameObject& object : scene->GetObjects())
    {
        AddObjectToTree(m_elementMap, object);
    }
}
