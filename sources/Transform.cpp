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

	// Yeah this whole "Delta change" thing is just bullshit, i'll fix that later wouldya ?

	mat4 scaling = glm::scale(mat4(1), 1.f + (scale - previousScale));
	mat4 translate = glm::translate(mat4(1),position - previousPosition);
	mat4 rotationMatrix = glm::rotate(glm::rotate(glm::rotate(mat4(1),rotation.z - previousRotation.z,vec3(0,0,1)),rotation.y - previousRotation.y,vec3(0,1,0)),rotation.x - previousRotation.x,vec3(1,0,0));

	modelTransform = rotationMatrix * translate  * scaling * modelTransform;

	previousPosition = position;
	previousScale = scale;
	previousRotation = rotation;
}
