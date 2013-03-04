#include "Transform.h"


Transform::Transform(void):
	position(vec3(0)), rotation(vec3(0)), scale(vec3(1)),
	previousPosition(vec3(0)),previousRotation(vec3(0)),previousScale(vec3(1))
{
}


Transform::~Transform(void)
{
}

void Transform::UpdateTransform()
{
	mat4 scaling = glm::scale(mat4(1), 1.f + (scale - previousScale));
	mat4 translate = glm::translate(mat4(1),position - previousPosition);

	modelTransform = modelTransform * translate * scaling;

	previousPosition = position;
	previousScale = scale;
}
