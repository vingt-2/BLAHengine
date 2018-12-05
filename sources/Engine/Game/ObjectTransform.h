#pragma once
#include "../../Common/Maths/Maths.h"
#include  "../../Common/StdInclude.h"

namespace BLAengine
{

    class BLACORE_API ObjectTransform
    {
    public:
        blaVec3 m_scale; 

        blaPosQuat m_transform;

        blaMat4 GetScaledTransformMatrix() const;

        blaVec3 GetEulerAngles() const;
        void SetEulerAngles(float xAngle, float yAngle, float zAngle);

        void SetRotation(blaQuat newRotation);
        blaQuat GetRotation() const;

        void SetPosition(blaVec3 newPosition);
        blaVec3 GetPosition() const;

        blaVec3 LocalDirectionToWorld(const blaVec3&  direction) const;
        blaVec3 LocalPositionToWorld(const blaVec3&  position) const;
        blaVec3 WorldDirectionToLocal(const blaVec3&  direction) const;
        blaVec3 WorldPositionToLocal(const blaVec3&  position) const;

        ObjectTransform(void);
        ~ObjectTransform(void);

    private:

    };

}