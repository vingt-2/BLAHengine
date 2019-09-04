#include "TestPlayerComponent.h"
#include <Engine/Core/DebugDraw.h>
#include <Engine/EngineInstance.h>
#include <Engine/System/InputManager.h>
#include <Engine/Core/Scene.h>
#include <Engine/Physics/RigidBodyComponent.h>
#include <Engine/Core/Timer.h>

using namespace BLAengine;

#define MAX(a,b) a > b ? a : b

BEGIN_COMPONENT_DESCRIPTION(TestPlayerComponent)
EXPOSE(m_speed)
EXPOSE(m_mainCameraObject)
END_DESCRIPTION()

TestPlayerComponent::TestPlayerComponent(GameObjectReference parentObject) :
    GameComponent(parentObject)
{}

TestPlayerComponent::~TestPlayerComponent()
{}

void TestPlayerComponent::Update()
{
    InputManager* inputs = InputManager::GetSingletonInstance();

    auto rigidBody = GetParentObject()->GetComponent<RigidBodyComponent>();

	if(!m_mainCameraObject.IsValid())
	{
		m_mainCameraObject  = EngineInstance::GetSingletonInstance()->GetWorkingScene()->GetMainCamera()->GetParentObject();
	}

    if (rigidBody)
    {
        if (!m_mainCameraObject.IsValid())
        {
            return;
        }
        if(m_mainCameraObject->GetParent()->GetName() == "Root")
        {
            m_mainCameraObject->SetParent(GetParentObject());
        }

        blaVec2 gxy = inputs->GetGamepadLeftAnalog().GetPosition();
		
		m_speed = 5 + 5.f * sinf(Timer::GetSingletonInstanceRead()->GetTime());

        blaVec3 controlInput = m_speed * m_mainCameraObject->GetTransform().LocalDirectionToWorld(blaVec3(gxy.x, 0, -gxy.y));

        controlInput.y = 0.f;
        
        DebugDraw::DrawRay(Ray(GetObjectTransform().GetPosition(), controlInput, 5.f * glm::length(controlInput)), BLAColors::LIME);

        if (inputs->GetGamepadState(BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_DOWN).IsRisingEdge())
        {
            rigidBody->AddImpulse(blaVec3(0.f, 5.f, 0.f));
        }

        float controlability = 0.05f;

        // On the ground ...
        if (GetObjectTransform().GetPosition().y < BLA_EPSILON)
        {
            controlability = 1.f;
            DebugDraw::DrawLine(GetObjectTransform().GetPosition(), GetObjectTransform().GetPosition() + blaVec3(0.f, abs(15.f * GetObjectTransform().GetPosition().y), 0.f));
            rigidBody->AddLinearForce(blaVec3(0.f, 5, 0.f));
            rigidBody->AddImpulse(blaVec3(0.f, MAX(-rigidBody->m_velocity.y, 0), 0.f));
            rigidBody->AddImpulse(-0.7f * blaVec3(rigidBody->m_velocity.x,0.f, rigidBody->m_velocity.z));
        }

        rigidBody->AddImpulse(controlability * controlInput);
    }
}