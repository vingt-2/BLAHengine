#include "Ray.h"
using namespace BLAengine;

Ray::Ray(blaVec3 origin, blaVec3 direction, float length)
{
    this->m_origin = origin;
    this->m_direction = normalize(direction);
    this->m_length = length;
}

blaVec3 Ray::GaussSolver(blaMat3 inMat, blaVec3 equal) const
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

RaycastHit Ray::RayToPlaneIntersection(blaVec3 planeOrigin, blaVec3 planeVec1, blaVec3 planeVec2) const
{
    blaVec3 solved = GaussSolver(blaMat3(-this->m_direction, planeVec1, planeVec2), (this->m_origin - planeOrigin));

    return RaycastHit(this->m_origin - solved.x * this->m_direction);
}
