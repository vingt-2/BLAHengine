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
	BeginComponentDeclaration(DemosLibrary, TestBlaComponent)
		void Update() override;

        blaVec3 color;

	EndComponentDeclaration()

	BeginComponentDescription(TestBlaComponent)
        Expose(color)
	EndComponentDescription()

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
