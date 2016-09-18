#pragma once
#include "../../../Common/Maths.h"
#include "..\..\..\Common\StdInclude.h"

class BLACORE_API RaycastHit
{
public:
	vec3 hitPosition;

	RaycastHit(vec3 hit)
	{
		hitPosition = hit;
	}
};

class BLACORE_API Ray
{
public:
	vec3 m_origin;
	vec3 m_direction;
	float m_length;

	Ray(vec3 origin, vec3 direction, float length);

	RaycastHit RayToPlaneIntersection(vec3 planeOrigin, vec3 planeVec1, vec3 planeVec2);

private:
	vec3 GaussSolver(mat3 inMat, vec3 equal);
};

