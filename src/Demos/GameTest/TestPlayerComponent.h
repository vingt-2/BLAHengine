#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>
#include <Engine/Animation/SkeletonAnimationData.h>

//TODO: Replace with contorller implementation from the input manager ...
#include <Engine/System/ControllerInputs/Dualshock4.h>

#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
BEGIN_COMPONENT_DECLARATION(TestPlayerComponent)
	void Update() override;

	TestPlayerComponent(GameObjectReference parentObject);
	~TestPlayerComponent(void) override;

	float m_speed = 1.f;
	GameObjectReference m_mainCameraObject;

	blaVec2 m_stickValue;

	float m_inAirFriction = 1.f;
	float m_onGroundFriction = 1.f;

	bool m_parentTheCamera = true;

	Dualshock4 m_controller;
END_DECLARATION()
}
