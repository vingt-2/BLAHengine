// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "StdInclude.h"
#include "Maths/Maths.h"

namespace BLA
{
    class CameraComponent;
    class RenderCamera
    {
    public:
        // Hold the View Projection matrix (updated each frame)
        blaMat4 m_worldToClipSpace;

        BLACORE_API virtual blaVec3 ClipSpaceToCameraDirection(blaVec2 clipSpaceCoord) = 0;

        BLACORE_API virtual void AttachCamera(CameraComponent* camera);
        BLACORE_API virtual void Update() = 0;

        BLACORE_API RenderCamera();

        // Points to the camera object
        CameraComponent* m_attachedCamera;
        float m_aspectRatio;
    };

    class PerspectiveCamera : public RenderCamera
    {
    public:
        blaMat4 m_perspectiveProjection;

        BLACORE_API void SetAspect(glm::vec2 renderSize);
        BLACORE_API void Update();
        blaVec3 ClipSpaceToCameraDirection(blaVec2 clipSpaceCoord) override;
    };

    class OrthographicCamera : public RenderCamera
    {
    public:
        blaMat4 m_orthographicProjection;

        BLACORE_API void SetOrthographicProj(float left, float right, float bottom, float top);
        BLACORE_API void Update();
        blaVec3 ClipSpaceToCameraDirection(blaVec2 clipSpaceCoord) override;
    };
}