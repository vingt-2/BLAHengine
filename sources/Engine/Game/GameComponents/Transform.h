#pragma once
#include "../../../Common/Maths.h"
#include  "../../../Common/StdInclude.h"

namespace BLAengine
{

    class BLACORE_API Transform
    {
    public:
        vec3 m_position;
        mat3 m_rotation;
        vec3 m_scale;

        mat4 m_transformMatrix;


        void UpdateTransform();
        void SetRotationUsingEuler(vec3 eulerAngles);
        vec3 GetEulerRotation() const;
        vec3 LocalDirectionToWorld(vec3 direction) const;
        vec3 LocalPositionToWorld(vec3 position) const;
        vec3 WorldDirectionToLocal(vec3 direction) const;
        vec3 WorldPositionToLocal(vec3 position) const;


        Transform(void);
        ~Transform(void);

    private:

    };

}