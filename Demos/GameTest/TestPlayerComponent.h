#pragma once

#include <Maths/Maths.h>
#include <Animation/SkeletonAnimationData.h>

//TODO: Replace with contorller implementation from the input manager ...
#include <System/ControllerInputs/Dualshock4.h>

#include <Core/GameComponent.h>

namespace BLAengine
{
BEGIN_COMPONENT_DECLARATION(DemosLibrary, TestPlayerComponent)

    void Init() override;
    void Update() override;
	
	float m_speed = 1.f;
	GameObject m_mainCameraObject;

	blaVec2 m_stickValue;

	float m_inAirFriction = 1.f;
	float m_onGroundFriction = 1.f;

	bool m_parentTheCamera = true;

	Dualshock4 m_controller;
END_DECLARATION()
}
