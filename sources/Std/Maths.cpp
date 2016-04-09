#include "Maths.h"

mat3 matrixCross(vec3 v)
{
	return mat3
	(
		vec3(0, -v[2], v[1]),
		vec3(v[2], 0, -v[0]),
		vec3(-v[1], v[0], 0)
	);
}