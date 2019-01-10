#include "CameraControl.h"

#include "../Engine/System/RenderWindow.h"
#include "../Engine/Game/GameComponents/CameraComponent.h"
#include "../Engine/Game/GameObject.h"
#include "../Engine/EngineInstance.h"
#include "../Engine/Game/Timer.h"

using namespace BLAengine;

//TODO: Rewrite DT independant if you want high stiffnesses...

void CameraController::UpdateController()
{
    if (m_controlledCamera == nullptr)
    {
        return;
    }

    ObjectTransform transform = m_controlledCamera->GetObjectTransform();
    GameObject* cameraObject = m_controlledCamera->GetParentObject();

    blaVec3 linearAcceleration(0.f);
    blaVec3 angularAcceleration(0.f);

    blaF32 multiplier = 1.f;
    if (m_renderWindow->GetKeyPressed(GLFW_KEY_LEFT_CONTROL))
    {
        multiplier *= 0.3f;
    }
    if (m_renderWindow->GetKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        multiplier *= 3.f;
    }

    if (m_renderWindow->GetKeyPressed('W'))
        linearAcceleration.z = -1.f;
    if (m_renderWindow->GetKeyPressed('S'))
        linearAcceleration.z = 1.f;

    if (m_renderWindow->GetKeyPressed('A'))
        linearAcceleration.x = -1.f;
    if (m_renderWindow->GetKeyPressed('D'))
        linearAcceleration.x = 1.f;

    if (m_renderWindow->GetKeyPressed('Q'))
        linearAcceleration.y = -1.f;
    if (m_renderWindow->GetKeyPressed('E'))
        linearAcceleration.y = 1.f;

    if (m_renderWindow->GetMousePressed(2))
    {
        double x, y;
        m_renderWindow->GetMouse(x, y);

        if (x - m_prevMousePosition.x > 0)
        {
            linearAcceleration.x += -1.f;
        }
        else if (x - m_prevMousePosition.x < 0)
        {
            linearAcceleration.x += 1.f;
        }
        if (y - m_prevMousePosition.y > 0)
        {
            linearAcceleration.y += 1.f;
        }
        else if (y - m_prevMousePosition.y < 0)
        {
            linearAcceleration.y += -1.f;
        }

        m_prevMousePosition = glm::vec2(x, y);
    }

    blaF32 mouseScroll;
    m_renderWindow->GetMouseWheel(mouseScroll);


    linearAcceleration.z -= multiplier * 30.f * (mouseScroll - m_lastScrollValue);

    m_lastScrollValue = mouseScroll;

    linearAcceleration = multiplier * 500.f * transform.LocalDirectionToWorld(linearAcceleration);

    if (m_renderWindow->GetMousePressed(1))
    {
        double x, y;
        m_renderWindow->GetMouse(x, y);
        glm::vec2 curMouse = glm::vec2(x, y);

        if (x - m_prevMousePosition.x > 0)
        {
            angularAcceleration.y = -1.f;
        }
        else if (x - m_prevMousePosition.x < 0)
        {
            angularAcceleration.y = 1.f;
        }
        if (y - m_prevMousePosition.y > 0)
        {
            angularAcceleration.x = -1.f;
        }
        else if (y - m_prevMousePosition.y < 0)
        {
            angularAcceleration.x = 1.f;
        }

        m_prevMousePosition = curMouse;

        angularAcceleration *= 400.f;

        angularAcceleration.x *= 9.0f/16.0f;
    }

    EngineInstance* engineInstance;

    BLA_RETRIEVE_SINGLETON(EngineInstance, engineInstance);

    blaF32 dt = engineInstance->GetTimer()->GetDelta();

    for (int i = 0; i < 2; ++i)
    {
        blaF32 dt = engineInstance->GetTimer()->GetDelta() / 2.f;

        m_cameraLinearVelocity += dt * (linearAcceleration - m_cameraDamping * m_cameraLinearVelocity);
        transform.SetPosition(transform.GetPosition() + dt * m_cameraLinearVelocity);

        m_cameraAngularVelocity += dt * (angularAcceleration - m_cameraDamping * m_cameraAngularVelocity);
        m_currentCameraEulerAngles += dt * m_cameraAngularVelocity;
        m_currentCameraEulerAngles.x = m_currentCameraEulerAngles.x > 0.45f * M_PI ? 0.45f * M_PI : m_currentCameraEulerAngles.x;
        m_currentCameraEulerAngles.x = m_currentCameraEulerAngles.x < -0.45f * M_PI ? -0.45f * M_PI : m_currentCameraEulerAngles.x;
        transform.SetEulerAngles(m_currentCameraEulerAngles);

        cameraObject->SetTransform(transform);
    }

    cameraObject->Update();
}