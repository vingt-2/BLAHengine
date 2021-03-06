#pragma once
#include "Maths.h"
#include "../StdInclude.h"

namespace BLA
{
    class RaycastHit
    {
    public:
        blaVec3 hitPosition;
        bool hasHit;
        RaycastHit(blaVec3 hit, bool hasHit)
        {
            hitPosition = hit;
            hasHit = hasHit;
        }
    };

    struct Ray
    {
        blaVec3 m_origin;
        blaVec3 m_direction;
        float m_length;

        Ray() : m_origin(blaVec3(0.f)), m_direction(blaVec3(0.f)), m_length(0.f) {}

        Ray(blaVec3 origin, blaVec3 direction, float length)
        {
            this->m_origin = origin;
            this->m_direction = normalize(direction);
            this->m_length = length;
        }
    };

    inline blaVec3 GaussSolver(blaMat3 inMat, blaVec3 equal)
    {
        glm::mat4x3 mat;
        mat[0] = inMat[0];
        mat[1] = inMat[1];
        mat[2] = inMat[2];
        mat[3] = equal;

        blaVec4 row0 = row(mat, 0) / mat[0][0];

        blaVec4 row1 = row(mat, 1) - (mat[0][1] * row0);
        row1 = row1 / row1[1];

        blaVec4 row2 = row(mat, 2) - (mat[0][2] * row0);
        row2 = row2 - (row2[1] * row1);
        row2 = row2 / row2[2];

        row1 = row1 - (row1[2] * row2);

        row0 = row0 - (row0[1] * row1) - (row0[2] * row2);

        return blaVec3(-row0[3], -row1[3], -row2[3]);
    };

    inline RaycastHit RayToPlaneIntersection(const Ray& ray, blaVec3 planeOrigin, blaVec3 planeNormal, blaF32& t)
    {
        float denom = abs(glm::dot(planeNormal, ray.m_direction));
        if (denom > 1e-4)
        {
            blaVec3 p0l0 = planeOrigin - ray.m_origin;
            t = abs(glm::dot(p0l0, planeNormal)) / denom;

            RaycastHit r(ray.m_origin + t * ray.m_direction, true);
            return r;
        }

        return RaycastHit(blaVec3(0), false);
    }

}
