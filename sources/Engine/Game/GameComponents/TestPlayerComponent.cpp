#include <Engine/Game/GameComponents/RigidBodyComponent.h>

#include "TestPlayerComponent.h"
#include <Engine/Debug/Debug.h>
#include <Engine/EngineInstance.h>
#include <Engine/Game/Scene.h>

#pragma optimize("",off)

using namespace BLAengine;

#define MAX(a,b) a > b ? a : b

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

    if (rigidBody)
    {
        GameObjectReference CameraObject;
        
        if (EngineInstance* engineInstance = EngineInstance::GetSingletonInstance())
        {
            CameraObject = engineInstance->GetWorkingScene()->GetMainCamera()->GetParentObject();
        }

        if (!CameraObject.IsValid())
        {
            return;
        }
        if(CameraObject->GetParent()->GetName() == "Root")
        {
            CameraObject->SetParent(GetParentObject());
        }

        blaVec3 controlInput = 10.f * CameraObject->GetTransform().LocalDirectionToWorld(blaVec3(jxz[0], 0.f, -jxz[1]));

        controlInput.y = 0.f;

        Debug* debug = Debug::GetSingletonInstance();

        debug->DrawRay(Ray(GetObjectTransform().GetPosition(), controlInput, 5.f * glm::length(controlInput)), BLAColors::LIME);

        if (m_controller.GetCircleButton())
        {
            rigidBody->AddImpulse(blaVec3(0.f, 0.1f, 0.f));
        }

        float controlability = 0.05f;

        // On the ground ...
        if (GetObjectTransform().GetPosition().y < BLA_EPSILON)
        {
            controlability = 1.f;
            debug->DrawLine(GetObjectTransform().GetPosition(), GetObjectTransform().GetPosition() + blaVec3(0.f, abs(15.f * GetObjectTransform().GetPosition().y), 0.f));
            rigidBody->AddLinearForce(blaVec3(0.f, 5, 0.f));
            rigidBody->AddImpulse(blaVec3(0.f, MAX(-rigidBody->m_velocity.y, 0), 0.f));
            rigidBody->AddImpulse(-0.7f * rigidBody->m_velocity);
        }

        rigidBody->AddImpulse(controlability * controlInput);
    }
}