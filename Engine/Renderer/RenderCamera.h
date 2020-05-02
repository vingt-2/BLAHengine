#pragma once

#include "Core/CameraComponent.h"

namespace BLA
{
    class BLACORE_API RenderCamera
    {
    public:
        // Hold the View Projection matrix (updated each frame)
        blaMat4 m_ViewProjection;

        virtual void AttachCamera(CameraComponent* camera);
        virtual void Update() = 0;

        RenderCamera();

        // Points to the camera object
        CameraComponent* m_attachedCamera;

    };

    class BLACORE_API PerspectiveCamera : public RenderCamera
    {
    public:
        blaMat4 m_perspectiveProjection;

        void SetPerspective(glm::vec2 renderSize);
        void Update();
    };

    class BLACORE_API OrthographicCamera : public RenderCamera
    {
    public:
        blaMat4 m_orthographicProjection;

        void SetOrthographicProj(float left, float right, float bottom, float top);
        void Update();
    };
}