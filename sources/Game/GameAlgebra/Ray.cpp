#include "Ray.h"

Ray::Ray(vec3 origin, vec3 direction, float length)
{
    this->origin = origin;
    this->direction = normalize(direction);
    this->length = length;
}

Ray::Ray(vec3 origin, vec3 endPoint)
{
    this->origin = origin;
    vec3 dir = (endPoint-origin);
    this->length = dir.length();
    this->direction = normalize(dir);
}

RaycastHit Ray::RayToPlaneIntersection(vec3 planeOrigin, vec3 planeVec1, vec3 planeVec2)
{
	vec3 solved = GaussSolver(mat3(-this->direction,planeVec1,planeVec2),(this->origin-planeOrigin));

	return RaycastHit(this->origin - solved.x * this->direction);
}