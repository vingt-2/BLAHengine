#pragma once

#include "Engine/Core/GameObject.h"
#include "Engine/Gui/GuiManager.h"

namespace BLAengine
{
	class ComponentInspectorGui
	{
		GameObjectReference m_selectedGameObject;
	
		BlaGuiWindow& m_window;

	public:
		ComponentInspectorGui() : m_window(*BlaGuiManager::GetSingletonInstance()->OpenWindow("Component Inspector")) {}

		void InspectGameObject(GameObjectReference gameObject);

		static blaVector<blaString> GetComponents(GameObjectReference gameObject);
	};
}
