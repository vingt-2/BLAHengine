// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "windows.h"
#include <immintrin.h>
#include "Maths/Maths.h"
#include "System/Console.h"
#include "Rendering/DebugDraw.h"
#include "Core/TransformComponent.h"
#include "Core/BehaviorComponent.h"

#include "Rendering/RenderPass.h"

// Must include include EngineInstance.h before BLA_PROJECT_DECLARE
#include "EngineInstance.h"
DLL_PROJECT_DECLARE

namespace BLA
{
	BeginBehaviorDeclaration(BLADemos, TestBlaComponent)

        blaVec3 color;
	
	EndBehaviorDeclaration()

	BeginBehaviorDescription(TestBlaComponent, Dependencies(RootSystem))
        Expose(color)
	EndBehaviorDescription()

	void TestBlaComponent::Update()
	{
        blaScaledTransform t = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
        DebugDraw::DrawOBB(t.GetPosQuat(), t.GetScale() * 0.5f, blaVec3(color));
	}

    DefineConsoleCommand(void, SayHi)
	{
        Console::LogMessage("Hi folks !");
	}
}
