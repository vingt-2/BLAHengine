#include "Ray.h"
using namespace BLAengine;

Ray::Ray(vec3 origin, vec3 direction, float length)
{
    this->m_origin = origin;
    this->m_direction = normalize(direction);
    this->m_length = length;
}

vec3 Ray::GaussSolver(mat3 inMat, vec3 equal)
{
    mat4x3 mat;
    mat[0] = inMat[0];
    mat[1] = inMat[1];
    mat[2] = inMat[2];
    mat[3] = equal;

    vec4 row0 = row(mat, 0) / mat[0][0];

    vec4 row1 = row(mat, 1) - (mat[0][1] * row0);
    row1 = row1 / row1[1];

    vec4 row2 = row(mat, 2) - (mat[0][2] * row0);
    row2 = row2 - (row2[1] * row1);
    row2 = row2 / row2[2];

    row1 = row1 - (row1[2] * row2);

    row0 = row0 - (row0[1] * row1) - (row0[2] * row2);

    return vec3(-row0[3], -row1[3], -row2[3]);
};

RaycastHit Ray::RayToPlaneIntersection(vec3 planeOrigin, vec3 planeVec1, vec3 planeVec2)
{
    vec3 solved = GaussSolver(mat3(-this->m_direction, planeVec1, planeVec2), (this->m_origin - planeOrigin));

    return RaycastHit(this->m_origin - solved.x * this->m_direction);
}
