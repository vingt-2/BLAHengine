#pragma once

#include "GameObjectInspectorGui.h"

#include <Gui/GuiElements.h>
#include <Core/GameComponent.h>

using namespace BLAengine;

void GameObjectInspector::InspectGameObject(GameObject gameObject)
{
    delete m_window.RootElement();

    m_selectedGameObject = gameObject;

    BlaGuiElement* root = new BlaGuiSimpleTextElement("", "");

    blaVector<blaString> componentNames = GetComponents(gameObject);

    for (GameComponent* comp : gameObject.GetAllComponents())
    {
        ComponentDescriptor compDescriptor = comp->GetComponentDescriptor();
        BlaGuiElement* compEl = new BlaGuiCollapsibleElement(blaString(compDescriptor.m_typeName));
        for (const ComponentDescriptor::ExposedMember& exposedMember : compDescriptor.m_members)
        {
            blaString memberTypeAndName = blaString(exposedMember.m_type->GetName()) + " " + blaString(exposedMember.m_name);
            BlaGuiElement* editElement = exposedMember.m_type->MakeEditGuiElement(memberTypeAndName, (char*)comp + exposedMember.m_offset);
            compEl->AddChild(editElement);
        }
        root->AddChild(compEl);
    }

    m_window.SetRootElement(root);
}

std::vector<blaString> GameObjectInspector::GetComponents(GameObject gameObject)
{
    blaVector<blaString> ret;

    for (GameComponent* comp : gameObject.GetAllComponents())
    {
        ret.push_back(blaString(comp->GetComponentDescriptor().m_typeName));
    }

    return ret;
}
