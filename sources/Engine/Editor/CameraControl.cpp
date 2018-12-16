#include "CameraControl.h"

#include "../System/RenderWindow.h"
#include "../Game/GameComponents/CameraComponent.h"
#include "../Game/GameObject.h"

using namespace BLAengine;

void CameraController::UpdateController()
{
    if (m_controlledCamera == nullptr)
    {
        return;
    }

    ObjectTransform transform = m_controlledCamera->GetObjectTransform();
    GameObject* cameraObject = m_controlledCamera->GetParentObject();

    blaVec3 tangentForce = blaVec3(0);
    blaVec3 movementForce = blaVec3(0);
    float coeff = 0.1f;

    bool hideMouse = false;

    if (m_renderWindow->GetKeyPressed('W'))
        tangentForce.z = -2.f;
    if (m_renderWindow->GetKeyPressed('S'))
        tangentForce.z = 2.f;

    if (m_renderWindow->GetKeyPressed('A'))
        tangentForce.x = -2.f;
    if (m_renderWindow->GetKeyPressed('D'))
        tangentForce.x = 2.f;

    if (m_renderWindow->GetKeyPressed('Q'))
        tangentForce.y = -2.f;
    if (m_renderWindow->GetKeyPressed('E'))
        tangentForce.y = 2.f;

    if (m_renderWindow->GetMousePressed(2))
    {
        double x, y;
        m_renderWindow->GetMouse(x, y);
        tangentForce = blaVec3(0, 0, 0);

        if (x - m_prevMousePosition.x > 0)
        {
            tangentForce.x = -1.f;
        }
        else if (x - m_prevMousePosition.x < 0)
        {
            tangentForce.x = 1.f;
        }
        if (y - m_prevMousePosition.y > 0)
        {
            tangentForce.y = 1.f;
        }
        else if (y - m_prevMousePosition.y < 0)
        {
            tangentForce.y = -1.f;
        }

        m_prevMousePosition = glm::vec2(x, y);
    }

    blaVec3 cameraForce = transform.LocalDirectionToWorld(tangentForce);
    cameraForce *= coeff;
    transform.SetPosition(transform.GetPosition() + cameraForce);

    if (m_renderWindow->GetMousePressed(1))
    {
        double x, y;
        m_renderWindow->GetMouse(x, y);
        glm::vec2 curMouse = glm::vec2(x, y);

        blaVec3 deltaRotation = blaVec3(0);

        if (x - m_prevMousePosition.x > 0)
        {
            deltaRotation.y = -1.f;
        }
        else if (x - m_prevMousePosition.x < 0)
        {
            deltaRotation.y = 1.f;
        }
        if (y - m_prevMousePosition.y > 0)
        {
            deltaRotation.x = -1.f;
        }
        else if (y - m_prevMousePosition.y < 0)
        {
            deltaRotation.x = 1.f;
        }

        m_prevMousePosition = curMouse;

        const blaVec3 eulerAngles = transform.GetEulerAngles();

        transform.SetEulerAngles(eulerAngles + 0.01f * deltaRotation);
    }

    cameraObject->SetTransform(transform);

    cameraObject->Update();
}