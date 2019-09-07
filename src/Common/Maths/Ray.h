#pragma once
#include <Common/Maths/Maths.h>
#include <Common/StdInclude.h>

namespace BLAengine
{
    class BLACORE_API RaycastHit
    {
    public:
        blaVec3 hitPosition;

        RaycastHit(blaVec3 hit)
        {
            hitPosition = hit;
        }
    };

    class BLACORE_API Ray
    {
    public:
        blaVec3 m_origin;
        blaVec3 m_direction;
        float m_length;
		Ray() : m_origin(blaVec3(0.f)), m_direction(blaVec3(0.f)), m_length(0.f) {}
        Ray(blaVec3 origin, blaVec3 direction, float length);

        RaycastHit RayToPlaneIntersection(blaVec3 planeOrigin, blaVec3 planeVec1, blaVec3 planevec2);

    private:
        blaVec3 GaussSolver(blaMat3 inMat, blaVec3 equal);
    };

}
