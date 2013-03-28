#pragma once

#include "Graphics.h"
#include "std.h"
class Transform
{
public:
	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 transformMatrix;


	void UpdateTransform();

	vec3 LocalDirectionToWorld(vec3 direction);
	vec3 LocalPositionToWorld(vec3 position);


	Transform(void);
	~Transform(void);

private:

};

