#include <Engine/Game/GameComponents/RigidBodyComponent.h>

#include "TestPlayerComponent.h"

#pragma optimize("",off)

using namespace BLAengine;

TestPlayerComponent::TestPlayerComponent(GameObjectReference parentObject) :
    GameComponent(parentObject)
{}

TestPlayerComponent::~TestPlayerComponent()
{}

void TestPlayerComponent::Update()
{
	if(!m_controller.GetDS4Found())
	{
		m_controller.Setup(ConnectionMode::USB);
	}

	if(!m_controller.GetDS4Found())
	{
		return;
	}

	m_controller.Update();

	blaVec2 jxz;
	m_controller.GetLeftJoystick(jxz[0], jxz[1]);

	auto rigidBody = GetParentObject()->GetComponent<RigidBodyComponent>();

	rigidBody->m_applyGravity = false;

	if(rigidBody)
	{
		rigidBody->PushForceWorld(GetObjectTransform().GetPosition(), GetObjectTransform().LocalDirectionToWorld(0.01f * blaVec3(jxz[0], 0, jxz[1])));
	}
}