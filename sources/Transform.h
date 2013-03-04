#pragma once

#include "Graphics.h"
#include "std.h"
class Transform
{
public:
	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 modelTransform;

	void UpdateTransform();

	Transform(void);
	~Transform(void);

private:
	vec3 previousPosition;
	vec3 previousRotation;
	vec3 previousScale;
};

