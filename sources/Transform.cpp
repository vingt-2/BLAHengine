#include "Transform.h"


Transform::Transform(void):
	position(vec3(0)), rotation(vec3(1)), scale(vec3(1))
{
}


Transform::~Transform(void)
{
}

mat4 Transform::GetTransform()
{
	mat4 currentTransform = mat4(1);
	currentTransform = glm::translate(currentTransform,position);
	return currentTransform;
}
