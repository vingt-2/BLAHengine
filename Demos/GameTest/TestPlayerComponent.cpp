#include "TestPlayerComponent.h"
#include <Core/DebugDraw.h>
#include <EngineInstance.h>
#include <System/InputManager.h>
#include <Core/Scene.h>
#include <Physics/RigidBodyComponent.h>
#include <Core/TransformComponent.h>
#include <Core/Timer.h>

using namespace BLAengine;

#define MAX(a,b) a > b ? a : b

BeginComponentDescription(TestPlayerComponent)
    Expose(m_speed)
    Expose(m_mainCameraObject)
    Expose(m_stickValue)
    Expose(m_inAirFriction)
    Expose(m_onGroundFriction)
    Expose(m_parentTheCamera)
EndComponentDescription()

void TestPlayerComponent::Init()
{
    m_stickValue = blaVec2(0);
}

void TestPlayerComponent::Update()
{
    InputManager* inputs = InputManager::GetSingletonInstance();

    auto rigidBody = GetOwnerObject().GetComponent<RigidBodyComponent>();

    if (!m_mainCameraObject.IsValid())
    {
        m_mainCameraObject = EngineInstance::GetSingletonInstance()->GetWorkingScene()->GetMainCamera()->GetOwnerObject();
    }

    if (rigidBody)
    {
        if (!m_mainCameraObject.IsValid())
        {
            return;
        }

        if (m_parentTheCamera)
        {
            m_mainCameraObject.SetParent(GetOwnerObject());
        }
        else
        {
            m_mainCameraObject.SetParent(GameObject::InvalidReference());
        }

        m_stickValue = inputs->GetGamepadLeftAnalog().GetPosition();

        blaScaledTransform cameraTransform = m_mainCameraObject.GetComponent<TransformComponent>()->GetTransform();

        blaVec3 controlInput = m_speed * cameraTransform.LocalDirectionToWorld(blaVec3(m_stickValue.x, 0, -m_stickValue.y));

        controlInput.y = 0.f;

        blaVec3 myPosition = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetPosition();

        DebugDraw::DrawRay(Ray(myPosition, controlInput, 5.f * glm::length(controlInput)), BLAColors::LIME);

        if (inputs->GetGamepadState(BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_DOWN).IsRisingEdge())
        {
            rigidBody->AddImpulse(blaVec3(0.f, 5.f, 0.f));
        }

        float controlability = 0.05f;

        // On the ground ...
        if (myPosition.y < BLA_EPSILON)
        {
            controlability = 1.f;
            DebugDraw::DrawLine(myPosition, myPosition + blaVec3(0.f, abs(15.f * myPosition.y), 0.f));
            rigidBody->AddLinearForce(blaVec3(0.f, 5, 0.f));
            rigidBody->AddImpulse(blaVec3(0.f, MAX(-rigidBody->m_velocity.y, 0), 0.f));
            rigidBody->AddImpulse(-m_onGroundFriction * blaVec3(rigidBody->m_velocity.x, 0.f, rigidBody->m_velocity.z));
        }
        else
        {
            rigidBody->AddImpulse(-m_inAirFriction * blaVec3(rigidBody->m_velocity.x, 0.f, rigidBody->m_velocity.z));
        }

        rigidBody->AddImpulse(controlability * controlInput);
    }
}
