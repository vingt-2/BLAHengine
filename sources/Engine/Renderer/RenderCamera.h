#pragma once
#include "../Game/GameComponents/Camera.h"

namespace BLAengine
{
    class BLACORE_API RenderCamera
    {
    public:
        // Hold the View Projection matrix (updated each frame)
        mat4 m_ViewProjection;

        virtual void AttachCamera(Camera* camera);
        virtual void Update() = 0;

        RenderCamera();

        // Points to the camera object
        Camera* m_attachedCamera;

    };

    class BLACORE_API PerspectiveCamera : public RenderCamera
    {
    public:
        mat4 m_perspectiveProjection;

        void SetPerspective(vec2 renderSize);
        void Update();
    };

    class BLACORE_API OrthographicCamera : public RenderCamera
    {
    public:
        mat4 m_orthographicProjection;

        void SetOrthographicProj(float left, float right, float bottom, float top);
        void Update();
    };
}