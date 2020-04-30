#pragma once
#include "Maths/Maths.h"

namespace BLA
{
    class RenderWindow;
    class CameraComponent;

    class CameraController
    {
    public:
        CameraController(RenderWindow* renderWindow,
            blaF32 damping,
            blaF32 maxVelocity) :

            m_cameraAngularVelocity(blaVec3(0.f)),
            m_cameraLinearVelocity(blaVec3(0.f)),
            m_cameraDamping(damping),
            m_currentCameraEulerAngles(0.f, 0.f, 0.f)
        {
            m_prevMousePosition = blaVec2(0.f);
        }

        ~CameraController()
        {};

        void UpdateController(blaF32 dt);

    private:

        blaIVec2 m_prevMousePosition;

        blaVec3 m_cameraAngularVelocity;
        blaVec3 m_cameraLinearVelocity;

        blaVec3 m_currentCameraEulerAngles;

        blaF32 m_cameraDamping;
        blaF32 m_maxAngularVelocity;
    };
}
