#pragma once

#include <Engine/Gui/GuiManager.h>
#include "Engine/Core/GameObject.h"

namespace BLAengine
{
	class SceneGraphGui
	{
		BlaGuiWindow& m_window;

	public:
		SceneGraphGui() : m_window(*BlaGuiManager::GetSingletonInstance()->OpenWindow("Scene Graph")) {}

		void UpdateSceneGraph();

	private:
		typedef blaMap<blaString, BlaGuiElement*> ElementMap;
		void AddObjectToTree(ElementMap& elementMap, const GameObjectReference& object);
	};
}