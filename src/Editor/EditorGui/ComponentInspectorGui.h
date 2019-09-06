#pragma once

#include "Engine/Core/GameObject.h"
#include "Engine/Gui/GuiManager.h"

namespace BLAengine
{
	class ComponentInspector
	{
		GameObjectReference m_selectedGameObject;
	
		BlaGuiWindow& m_window;

	public:
		ComponentInspector() : m_window(*BlaGuiManager::GetSingletonInstance()->OpenWindow("Component Inspector")) {}

		void InspectGameObject(GameObjectReference gameObject);

		static blaVector<blaString> GetComponents(GameObjectReference gameObject);
	};
}
