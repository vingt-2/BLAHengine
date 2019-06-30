#include "CameraControl.h"

#include <Engine/Game/GameComponents/CameraComponent.h>
#include <Engine/Game/GameObject.h>
#include <Engine/EngineInstance.h>
#include <Engine/Game/Timer.h>
#include <Engine/System/InputManager.h>

using namespace BLAengine;

//TODO: Rewrite DT independant if you want high stiffnesses...

void CameraController::UpdateController()
{
    auto inputs = InputManager::GetSingletonInstanceRead();

    if (m_controlledCamera == nullptr)
    {
        return;
    }

    ObjectTransform transform = m_controlledCamera->GetLocalObjectTransform();
    GameObjectReference cameraObject = m_controlledCamera->GetParentObject();

    blaVec3 linearAcceleration(0.f);
    blaVec3 angularAcceleration(0.f);

    blaF32 multiplier = 1.f;
    if (inputs->GetKeyState(BLA_KEY_LEFT_CONTROL).IsDown())
    {
        multiplier *= 0.3f;
    }
    if (inputs->GetKeyState(BLA_KEY_LEFT_SHIFT).IsDown())
    {
        multiplier *= 3.f;
    }

    if (inputs->GetKeyState(BLA_KEY_W).IsDown())
        linearAcceleration.z = -1.f;
    if (inputs->GetKeyState(BLA_KEY_S).IsDown())
        linearAcceleration.z = 1.f;

    if (inputs->GetKeyState(BLA_KEY_A).IsDown())
        linearAcceleration.x = -1.f;
    if (inputs->GetKeyState(BLA_KEY_D).IsDown())
        linearAcceleration.x = 1.f;

    if (inputs->GetKeyState(BLA_KEY_Q).IsDown())
        linearAcceleration.y = -1.f;
    if (inputs->GetKeyState(BLA_KEY_E).IsDown())
        linearAcceleration.y = 1.f;

    const blaVec2 mouseDelta = inputs->GetMousePointerState().GetDelta();

    auto middleMouseButton = inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_MIDDLE);
    if (middleMouseButton.IsDown())
    {
        if (mouseDelta.x > 0)
        {
            linearAcceleration.x += -1.f;
        }
        else if (mouseDelta.x < 0)
        {
            linearAcceleration.x += 1.f;
        }
        if (mouseDelta.y > 0)
        {
            linearAcceleration.y += 1.f;
        }
        else if (mouseDelta.y < 0)
        {
            linearAcceleration.y += -1.f;
        }
    }

    blaF32 mouseScrollDelta = inputs->GetMouseScrollDelta();

    linearAcceleration.z -= multiplier * 30.f * mouseScrollDelta;


    linearAcceleration = multiplier * 500.f * transform.LocalDirectionToWorld(linearAcceleration);

    auto rightButtonState = inputs->GetMouseButtonTimedState(BLA_MOUSE_BUTTON_RIGHT);
    if (rightButtonState.IsDown())
    {
        if (mouseDelta.x > 0)
        {
            angularAcceleration.y = -1.f;
        }
        else if (mouseDelta.x < 0)
        {
            angularAcceleration.y = 1.f;
        }
        if (mouseDelta.y > 0)
        {
            angularAcceleration.x = -1.f;
        }
        else if (mouseDelta.y < 0)
        {
            angularAcceleration.x = 1.f;
        }

        angularAcceleration *= 200.f;

        angularAcceleration.x *= 9.0f/16.0f;
    }

    for (int i = 0; i < 3; ++i)
    {
        blaF32 dt = Timer::GetSingletonInstance()->GetDelta();

        dt /= 3;

        m_cameraLinearVelocity += dt * (linearAcceleration - m_cameraDamping * m_cameraLinearVelocity);
        transform.SetPosition(transform.GetPosition() + dt * m_cameraLinearVelocity);

        m_cameraAngularVelocity += dt * (angularAcceleration - m_cameraDamping * m_cameraAngularVelocity);
        m_currentCameraEulerAngles += dt * m_cameraAngularVelocity;
        m_currentCameraEulerAngles.x = m_currentCameraEulerAngles.x > 0.45f * M_PI ? 0.45f * M_PI : m_currentCameraEulerAngles.x;
        m_currentCameraEulerAngles.x = m_currentCameraEulerAngles.x < -0.45f * M_PI ? -0.45f * M_PI : m_currentCameraEulerAngles.x;
        transform.SetEulerAngles(m_currentCameraEulerAngles);

        cameraObject->SetLocalTransform(transform);
    }

    cameraObject->Update();
}