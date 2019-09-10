#include "SceneGraphGui.h"

#include <Engine/Core/Scene.h>
#include <Engine/EngineInstance.h>

using namespace BLAengine;

#pragma optimize("", off)

void SceneGraphGui::AddObjectToTree(ElementMap& elementMap, const GameObjectReference& object)
{
	auto root = m_window.RootElement();

	ElementMap::iterator it = elementMap.find(object->GetName());

	if(it != elementMap.end())
	{
		return;
	}

	BlaGuiElement* element = new BlaGuiCollapsibleElement(object->GetName());
	if (object->GetParent().IsValid())
	{
		ElementMap::iterator parentIt = elementMap.find(object->GetParent()->GetName());

		if (parentIt != elementMap.end())
		{
			root->AddChildAfterNode(element, parentIt->second);
		}
		else
		{
			AddObjectToTree(elementMap, object->GetParent());
		}
	}
	else
	{
		root->AddChild(element);
	}

	elementMap.insert(blaPair<blaString, BlaGuiElement*>(object->GetName(), element));
}

void SceneGraphGui::UpdateSceneGraph()
{
	Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();

	delete m_window.RootElement();

	BlaGuiElement* root = new BlaGuiSimpleTextElement("SceneRoot", "Scene Objects: ");

	m_window.SetRootElement(root);

	ElementMap elementMap;

	for(const GameObjectReference& object : scene->GetObjects())
	{
		AddObjectToTree(elementMap, object);
	}
}
