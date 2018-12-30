#pragma once
#include "../Common/Maths/Maths.h"

namespace BLAengine
{
    class RenderWindow;
    class CameraComponent;

    class CameraController
    {
    public:
        CameraController(RenderWindow* renderWindow,
            CameraComponent* controlledCamera,
            blaF32 damping,
            blaF32 maxVelocity) :

            m_cameraAngularVelocity(blaVec3(0.f)),
            m_cameraDamping(damping),
            m_maxAngularVelocity(maxVelocity),
            m_currentCameraEulerAngles(0.f, 0.f, 0.f),
            m_lastScrollValue(0.f)
        {
            m_renderWindow = renderWindow;
            m_controlledCamera = controlledCamera;
        }

        ~CameraController();

        void UpdateController();

    private:
        CameraComponent* m_controlledCamera;
        const RenderWindow* m_renderWindow;
    
        blaIVec2 m_prevMousePosition;
        blaF32 m_lastScrollValue;

        blaVec3 m_cameraAngularVelocity;
        blaVec3 m_cameraLinearVelocity;

        blaVec3 m_currentCameraEulerAngles;

        blaF32 m_cameraDamping;
        blaF32 m_maxAngularVelocity;
    };
}
