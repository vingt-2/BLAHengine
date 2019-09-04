#pragma once

#include <Engine/Gui/GuiManager.h>
#include "Engine/Core/GameObject.h"

namespace BLAengine
{
	class SceneGraphGui
	{
		BlaGuiWindow& m_window;

	public:
		SceneGraphGui() : m_window(BlaGuiManager::GetSingletonInstance()->OpenWindow("Scene Graph")) {}

		void Update();
	};
}