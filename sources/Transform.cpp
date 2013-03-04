#include "Transform.h"


Transform::Transform(void):
	position(vec3(0)), rotation(vec3(1)), scale(vec3(1,1,1))
{
}


Transform::~Transform(void)
{
}

mat4 Transform::GetTransform()
{
	mat4 currentTransform = mat4(1);
	mat4 scaling = glm::scale(mat4(1),scale);
	mat4 translate = glm::translate(mat4(1),position);

	currentTransform = translate * scaling;
	return currentTransform;
}
