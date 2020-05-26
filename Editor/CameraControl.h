// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

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

            m_cameraEulerVelocity(blaVec3(0.f)),
            m_cameraLinearVelocity(blaVec3(0.f)),
            m_cameraDamping(damping),
            m_currentCameraEulerAngles(0.f, 0.f, 0.f),
            m_homingTowardsFocus(false)
        {
            m_prevMousePosition = blaVec2(0.f);
        }

        ~CameraController()
        {};

        void UpdateController(blaF32 dt);

        void SetTransformFocus(blaPosQuat focus);

    private:
        bool m_homingTowardsFocus;

        blaVec3 m_desiredPos;
        blaVec3 m_desiredRotEuler;
        blaQuat m_desiredRot;

        blaIVec2 m_prevMousePosition;

        blaVec3 m_cameraEulerVelocity;
        blaVec3 m_cameraLinearVelocity;

        blaVec3 m_currentCameraEulerAngles;

        blaF32 m_cameraDamping;
        blaF32 m_eulerDamping;
    };
}
