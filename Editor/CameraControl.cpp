// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "CameraControl.h"

#include "Core/CameraComponent.h"
#include "Core/TransformComponent.h"
#include "Core/GameObject.h"
#include "Core/Timer.h"
#include "System/InputManager.h"
#include "Core/Scene.h"

using namespace BLA;

//TODO: Rewrite DT independant if you want high stiffnesses...

#define MAX_MOUSE_DELTA 30.f // <- this will clamp camera sensitivity ...

void CameraController::UpdateController(blaF32 dt)
{
    auto inputs = InputManager::GetSingletonInstanceRead();

    CameraComponent* camera = Core::Scene::GetSingletonInstance()->GetMainCamera();

    if (!camera) return;

    Core::GameObject cameraObject = camera->GetOwnerObject(); 
    
    TransformComponent* transformComponent = cameraObject.GetComponent<TransformComponent>();
    blaScaledTransform transform = transformComponent->GetTransform();

    blaVec3 linearAcceleration(0.f);
    blaVec3 eulerAcceleration(0.f);

    if(inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_MIDDLE).IsDown() || 
        inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_LEFT).IsDown() ||
        inputs->GetMouseButtonState(BLA_MOUSE_BUTTON_RIGHT).IsDown()) 
    {
        m_homingTowardsFocus = false;
    }

    if(m_homingTowardsFocus) 
    {
        linearAcceleration = 5.f * (m_desiredPos - transform.GetPosition());
        eulerAcceleration = 3000.f * (m_desiredRotEuler - m_currentCameraEulerAngles);

        if(glm::length2(linearAcceleration) < 0.001f)
            m_homingTowardsFocus = false;
    }

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

    blaVec2 mouseDelta = inputs->GetMousePointerState().GetDelta();

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

    for (int i = 0; i < 3; ++i)
    {
        dt /= 3;

        m_cameraLinearVelocity += dt * (linearAcceleration - m_cameraDamping * m_cameraLinearVelocity);
        transform.SetPosition(transform.GetPosition() + dt * m_cameraLinearVelocity);

        m_cameraEulerVelocity += dt * (eulerAcceleration - m_cameraDamping * m_cameraEulerVelocity);

        mouseDelta.x = mouseDelta.x > MAX_MOUSE_DELTA ? MAX_MOUSE_DELTA : mouseDelta.x;
        mouseDelta.x = mouseDelta.x < -MAX_MOUSE_DELTA ? -MAX_MOUSE_DELTA : mouseDelta.x;

        mouseDelta.y = mouseDelta.y > MAX_MOUSE_DELTA ? MAX_MOUSE_DELTA : mouseDelta.y;
        mouseDelta.y = mouseDelta.y < -MAX_MOUSE_DELTA ? -MAX_MOUSE_DELTA : mouseDelta.y;

        if (inputs->GetMouseButtonTimedState(BLA_MOUSE_BUTTON_RIGHT).IsRisingEdge())
        {
            return;
        }

        // Let's keep input integration for controlling the camera with a joystick ...
        auto rightButtonState = inputs->GetMouseButtonTimedState(BLA_MOUSE_BUTTON_RIGHT);
        if (rightButtonState.IsDown())
        {
            m_currentCameraEulerAngles += dt * -1.0f * blaVec3(9.0f * mouseDelta.y / 16.f, mouseDelta.x, 0.f); // TODO: Get Aspect ratio...
            m_currentCameraEulerAngles.x = m_currentCameraEulerAngles.x > 0.45f * M_PI ? 0.45f * M_PI : m_currentCameraEulerAngles.x;
            m_currentCameraEulerAngles.x = m_currentCameraEulerAngles.x < -0.45f * M_PI ? -0.45f * M_PI : m_currentCameraEulerAngles.x;
        }
        else 
        {
            m_currentCameraEulerAngles += m_cameraEulerVelocity * dt;
        }

        linearAcceleration = blaVec3(0);
        eulerAcceleration = blaVec3(0);

        transform.SetEulerAngles(m_currentCameraEulerAngles);

        transformComponent->SetTransform(transform);
    }
}

void CameraController::SetTransformFocus(blaPosQuat focus)
{
    m_desiredPos = focus.TransformPoint(blaVec3(0.f, 0.0f, 1.f));
    m_desiredRot = QuatBetweenVectors(blaVec3(0.f, 0.f, 1.f), m_desiredPos - focus.GetTranslation3());

    m_desiredRotEuler = blaPosQuat::QuatToEuler(m_desiredRot);
    m_desiredRotEuler.z = 0; // Kill roll because we don't control roll in the editor. That would really suck if we got stuck rolling ...
    m_homingTowardsFocus = true;
}
