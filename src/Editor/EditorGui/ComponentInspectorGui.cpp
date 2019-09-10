#pragma once

#include "ComponentInspectorGui.h"
#include <Engine/Gui/GuiElements.h>
#include <Engine/Core/GameComponent.h>

using namespace BLAengine;

void ComponentInspectorGui::InspectGameObject(GameObjectReference gameObject) 
{
	delete m_window.RootElement();

	m_selectedGameObject = gameObject;

	BlaGuiElement* root = new BlaGuiSimpleTextElement("", "Components: ");

	std::vector<blaString> componentNames = GetComponents(gameObject);

	for (GameComponent* comp : gameObject->GetAllComponents())
	{
		ComponentReflection::ComponentDescriptor compDescriptor = comp->GetComponentDescriptor();
		BlaGuiElement* compEl = new BlaGuiCollapsibleElement(compDescriptor.m_typeName);
		for(const ComponentReflection::ComponentDescriptor::ExposedMember& exposedMember : compDescriptor.m_members) 
		{
			blaString memberTypeAndName = exposedMember.m_type->GetName() + " " + exposedMember.m_name;
			BlaGuiElement* editElement = exposedMember.m_type->MakeEditGuiElement(memberTypeAndName, (char*) comp + exposedMember.m_offset);
			compEl->AddChild(editElement);
		}
		root->AddChild(compEl);
	}

	m_window.SetRootElement(root);
}

std::vector<blaString> ComponentInspectorGui::GetComponents(GameObjectReference gameObject)
{
	blaVector<blaString> ret;

	for(GameComponent* comp : gameObject->GetAllComponents()) 
	{
		ret.push_back(comp->GetComponentDescriptor().m_typeName);
	}

	return ret;
}
