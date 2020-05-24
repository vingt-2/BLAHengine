#pragma once

#include "GameObjectInspectorGui.h"

#include "EditorGui/InspectableVariablesGuiElements.h"
#include "Gui/GuiManager.h"
#include "Gui/GuiElements.h"
#include "Core/GameComponent.h"
#include "EditorCommands.h"


#define OBJECT_INSPECTOR_ELEMENT_GROUP_ID BlaStringId("ObjectInspectorElement")

using namespace BLA;

GameObjectInspector::GameObjectInspector(EditorCommandManager* editorCommandManager): m_window(
    *BlaGuiManager::GetSingletonInstance()->OpenWindow("GameObject Inspector"))
,   m_editorCommandManager(editorCommandManager)
{
}

void GameObjectInspector::InspectGameObject(GameObject gameObject)
{
    delete m_window.RootElement();

    m_selectedGameObject = gameObject;

    BlaGuiElement* root = new BlaGuiSimpleTextElement("", OBJECT_INSPECTOR_ELEMENT_GROUP_ID, "");

    blaVector<blaString> componentNames = GetComponents(gameObject);

    for (GameComponent* comp : gameObject.GetAllComponents())
    {
        ComponentDescriptor compDescriptor = comp->GetComponentDescriptor();
        BlaGuiCollapsibleElement* compEl = new BlaGuiCollapsibleElement(blaString(compDescriptor.m_typeID), OBJECT_INSPECTOR_ELEMENT_GROUP_ID);
        compEl->m_decorateHeader = true;
        for (const ComponentDescriptor::ExposedMember& exposedMember : compDescriptor.m_members)
        {
            blaString memberName = blaString(exposedMember.m_name);
            if (memberName.find("m_") == 0) memberName = memberName.substr(2);
            memberName[0] = std::toupper(memberName[0]);
            
            BlaGuiElement* editElement = InspectableVariablesEditorGuiElementFactoryManager::MakeEditGuiElement(
                memberName, 
                BlaStringId("ComponentExposeEditing"), 
                exposedMember.m_type,
                [commandManager = m_editorCommandManager](void*)
                {
                    commandManager->Execute();
                },
                (char*)comp + exposedMember.m_offset);

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
        ret.push_back(blaString(comp->GetComponentDescriptor().m_typeID));
    }

    return ret;
}
