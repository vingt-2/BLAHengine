#pragma once
#include "../../Common/Maths/Maths.h"

namespace BLAengine
{
    class RenderWindow;
    class CameraComponent;

    class CameraController
    {
    public:
        CameraController(RenderWindow* renderWindow, CameraComponent* controlledCamera)
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
    };
}
