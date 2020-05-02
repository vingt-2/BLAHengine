#pragma once

#include "GameObjectInspectorGui.h"

#include "Gui/GuiElements.h"
#include "Core/GameComponent.h"

#define OBJECT_INSPECTOR_ELEMENT_GROUP_ID BlaStringId("ObjectInspectorElement")

using namespace BLA;

void GameObjectInspector::InspectGameObject(GameObject gameObject)
{
    delete m_window.RootElement();

    m_selectedGameObject = gameObject;

    BlaGuiElement* root = new BlaGuiSimpleTextElement("", OBJECT_INSPECTOR_ELEMENT_GROUP_ID, "");

    blaVector<blaString> componentNames = GetComponents(gameObject);

    for (GameComponent* comp : gameObject.GetAllComponents())
    {
        ComponentDescriptor compDescriptor = comp->GetComponentDescriptor();
        BlaGuiElement* compEl = new BlaGuiCollapsibleElement(blaString(compDescriptor.m_typeName), OBJECT_INSPECTOR_ELEMENT_GROUP_ID);
        for (const ComponentDescriptor::ExposedMember& exposedMember : compDescriptor.m_members)
        {
            blaString memberTypeAndName = blaString(exposedMember.m_type->GetName()) + " " + blaString(exposedMember.m_name);
            BlaGuiElement* editElement = exposedMember.m_type->MakeEditGuiElement(memberTypeAndName, BlaStringId("ComponentExposeEditing"), (char*)comp + exposedMember.m_offset);
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
