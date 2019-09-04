#include "SceneGraphGui.h"

#include <Engine/Core/Scene.h>
#include <Engine/EngineInstance.h>

using namespace BLAengine;

void SceneGraphGui::Update()
{
	const Scene& scene = *EngineInstance::GetSingletonInstance()->GetSingletonInstance()->GetWorkingScene();
}
