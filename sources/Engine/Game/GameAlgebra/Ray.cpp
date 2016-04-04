#include "Ray.h"

Ray::Ray(vec3 origin, vec3 direction, float length)
{
	this->m_origin = origin;
	this->m_direction = normalize(direction);
	this->m_length = length;
}

Ray::Ray(vec3 origin, vec3 endPoint)
{
	this->m_origin = origin;
	vec3 dir = (endPoint-origin);
	this->m_length = dir.length();
	this->m_direction = normalize(dir);
}

RaycastHit Ray::RayToPlaneIntersection(vec3 planeOrigin, vec3 planeVec1, vec3 planeVec2)
{
	vec3 solved = GaussSolver(mat3(-this->m_direction, planeVec1, planeVec2), (this->m_origin - planeOrigin));

	return RaycastHit(this->m_origin - solved.x * this->m_direction);
}
