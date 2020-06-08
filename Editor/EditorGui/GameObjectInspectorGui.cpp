// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "GameObjectInspectorGui.h"

#include "EditorGui/InspectableVariablesGuiElements.h"
#include "Gui/DevGuiManager.h"
#include "Gui/DevGuiElements.h"
#include "Core/TransformComponent.h"
#include "EditorCommands.h"

//TODO: This is starting to smell. Most especially the custom gui element for the transform component ...

#define OBJECT_INSPECTOR_ELEMENT_GROUP_ID BlaStringId("ObjectInspectorElement")

using namespace BLA;

class TransformComponentCustomGuiElement : public DevGuiCollapsibleElement
{
    TransformComponent* m_pToTransformComponent;
public:
    TransformComponentCustomGuiElement(EditorCommandManager* editorCommandManager, TransformComponent* transformComponent) :
	    DevGuiCollapsibleElement("TransformComponent", OBJECT_INSPECTOR_ELEMENT_GROUP_ID), m_pToTransformComponent(transformComponent)
    {
        DevGuiElement* editElement = InspectableVariablesEditorGuiElementFactoryManager::MakeEditGuiElement(
            "World Transform",
            BlaStringId("ComponentExposeEditing"),
            BLAInspectableVariables::TypeResolver<blaScaledTransform>::GetDescriptor(),
            [commandManager = editorCommandManager, objId = transformComponent->GetOwnerObject().GetId(), compId = BlaStringId("TransformComponent"), memberId = BlaStringId("WorldTransform")]
		        (const char* newValue, const char* preValue, blaIndex sizeOfValue)
		        {
		            commandManager->Execute(new GameComponentEditCommand(objId, compId, memberId, ValueCommandDelta(preValue, newValue, sizeOfValue)));
		        },
            (char*)&m_worldTransform);

        AddChild(editElement);
    }

	void Render() override
    {
        m_worldTransform = m_pToTransformComponent->GetTransform();
	
    	DevGuiCollapsibleElement::Render();
    }

    blaScaledTransform m_worldTransform;
};

GameObjectInspector::GameObjectInspector(EditorCommandManager* editorCommandManager): m_window(
    *DevGuiManager::GetSingletonInstance()->OpenWindow("GameObject Inspector"))
,   m_editorCommandManager(editorCommandManager)
{
}

void GameObjectInspector::InspectGameObject(GameObject gameObject)
{
    delete m_window.RootElement();

    m_selectedGameObject = gameObject;

    DevGuiElement* root = new DevGuiSimpleTextElement("", OBJECT_INSPECTOR_ELEMENT_GROUP_ID, "");

    // Refresh cached World transform to be sure we display the right one :)
    m_selectedGameObject.GetComponent<TransformComponent>()->GetTransform();
	
    for (GameComponent* comp : gameObject.GetAllComponents())
    {
        ComponentDescriptor compDescriptor = comp->GetComponentDescriptor();
        DevGuiCollapsibleElement* compEl;
    	if(compDescriptor.m_typeID == BlaStringId("TransformComponent"))
    	{
            compEl = new TransformComponentCustomGuiElement(m_editorCommandManager, static_cast<TransformComponent*>(comp));
            compEl->m_decorateHeader = true;
    	}
        else
        {
	        compEl = new DevGuiCollapsibleElement(blaString(compDescriptor.m_typeID), OBJECT_INSPECTOR_ELEMENT_GROUP_ID);
            compEl->m_decorateHeader = true;
            for (const ComponentDescriptor::ExposedMember& exposedMember : compDescriptor.m_members)
            {
                blaString memberName = blaString(exposedMember.m_name);
                if (memberName.find("m_") == 0) memberName = memberName.substr(2);
                memberName[0] = std::toupper(memberName[0]);

                DevGuiElement* editElement = InspectableVariablesEditorGuiElementFactoryManager::MakeEditGuiElement(
                    memberName,
                    BlaStringId("ComponentExposeEditing"),
                    exposedMember.m_type,
                    [commandManager = m_editorCommandManager, objId = gameObject.GetId(), compId = compDescriptor.m_typeID, memberId = exposedMember.m_name]
                (const char* newValue, const char* preValue, blaIndex sizeOfValue)
                {
                    commandManager->Execute(new GameComponentEditCommand(objId, compId, memberId, ValueCommandDelta(preValue, newValue, sizeOfValue)));
                },
                    (char*)comp + exposedMember.m_offset);

                compEl->AddChild(editElement);
            }
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
