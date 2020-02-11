#include <windows.h>

#include <Maths/Maths.h>
#include "Core/GameComponent.h"
#include <System/Console.h>
#include <Core/DebugDraw.h>
#include <Core/TransformComponent.h>


// Must include include EngineInstance.h
#include <EngineInstance.h>
DLL_PROJECT_DECLARE

namespace BLAengine
{
	BEGIN_COMPONENT_DECLARATION(DemosLibrary, TestBlaComponent)
		void Update() override;

        blaVec3 color;

	END_DECLARATION()

	BEGIN_COMPONENT_DESCRIPTION(TestBlaComponent)
        EXPOSE(color)
	END_DESCRIPTION()

	void TestBlaComponent::Update()
	{
        blaScaledTransform t = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
        DebugDraw::DrawOBB(t.GetPosQuat(), t.GetScale() * 0.5f, blaVec3(color));
	}

    BLA_CONSOLE_COMMAND(void, SayHi)
	{
        Console::LogMessage("Hi folks !");
	}
}
