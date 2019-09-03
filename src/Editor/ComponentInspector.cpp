#pragma once

#include "ComponentInspector.h"
#include <Engine/Gui/GuiElements.h>
#include <Engine/Core/GameComponent.h>

using namespace BLAengine;

void ComponentInspector::InspectGameObject(GameObjectReference gameObject) 
{
	delete m_window.RootElement();

	m_selectedGameObject = gameObject;

	BlaGuiElement* root = new BlaGuiSimpleTextElement("", "Components: ");

	std::vector<blaString> componentNames = GetComponents(gameObject);

	for(auto name : componentNames) 
	{
		root->AddChild(new BlaGuiSimpleTextElement(name, "I am a " + name));
	}
	
	m_window.SetRootElement(root);
}

std::vector<blaString> ComponentInspector::GetComponents(GameObjectReference gameObject)
{
	blaVector<blaString> ret;

	for(GameComponent* comp : gameObject->GetAllComponents()) 
	{
		ret.push_back(comp->GetComponentDescriptor().m_typeName);
	}

	return ret;
}
