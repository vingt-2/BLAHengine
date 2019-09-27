#pragma once

#include <Engine/Gui/GuiManager.h>
#include "Engine/Core/GameObject.h"
#include "Engine/Gui/GuiElements.h"

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

		void OnGuiElementSelected(const BlaGuiElementEventPayload& event);

		static void StaticOnGuiElementSelected(void* thisPtr, const BlaGuiElementEventPayload& event) { ((SceneGraphGui*)thisPtr)->OnGuiElementSelected(event); }

	};
}
