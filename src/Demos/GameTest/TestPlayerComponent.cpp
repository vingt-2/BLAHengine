#include "TestPlayerComponent.h"
#include <Engine/Core/DebugDraw.h>
#include <Engine/EngineInstance.h>
#include <Engine/System/InputManager.h>
#include <Engine/Core/Scene.h>
#include <Engine/Physics/RigidBodyComponent.h>

using namespace BLAengine;

#define MAX(a,b) a > b ? a : b

BEGIN_COMPONENT_DESCRIPTION(TestPlayerComponent)
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

    if (rigidBody)
    {
        GameObjectReference CameraObject = EngineInstance::GetSingletonInstance()->GetWorkingScene()->GetMainCamera()->GetParentObject();

        if (!CameraObject.IsValid())
        {
            return;
        }
        if(CameraObject->GetParent()->GetName() == "Root")
        {
            CameraObject->SetParent(GetParentObject());
        }

        blaVec2 gxy = inputs->GetGamepadLeftAnalog().GetPosition();

        blaVec3 controlInput = 10.f * CameraObject->GetTransform().LocalDirectionToWorld(blaVec3(gxy.x, 0, -gxy.y));

        controlInput.y = 0.f;
        
        DebugDraw::DrawRay(Ray(GetObjectTransform().GetPosition(), controlInput, 5.f * glm::length(controlInput)), BLAColors::LIME);

        if (inputs->GetGamepadState(BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_DOWN).IsRisingEdge())
        {
            rigidBody->AddImpulse(blaVec3(0.f, 10.f, 0.f));
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