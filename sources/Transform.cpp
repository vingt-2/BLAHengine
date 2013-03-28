#include "Transform.h"


Transform::Transform(void):
	position(vec3(0)), rotation(vec3(0)), scale(vec3(1))
{
}


Transform::~Transform(void)
{
}

void Transform::UpdateTransform()
{
	mat4 translationMatrix = mat4(
								vec4(1,0,0,0),
								vec4(0,1,0,0),
								vec4(0,0,1,0),
								vec4(position.x,position.y,position.z,1)
								);
	GLfloat phi = rotation.x;
	GLfloat theta = rotation.y;
	GLfloat psy = rotation.z;


	// Yep, I'm using a good old fashion rotation matrix. So what ?
	// I'll get to quaternions later.
	mat4 rotationMatrix = mat4(
							vec4(cos(theta)*cos(psy),-cos(theta)*sin(psy),sin(theta),0),
							vec4(cos(phi)*sin(psy) + sin(phi)*sin(theta)*cos(psy),cos(phi)*cos(psy) - sin(phi)*sin(theta)*sin(psy),-sin(phi)*cos(theta),0),
							vec4(sin(phi)*sin(psy) - cos(phi)*sin(theta)*cos(psy),sin(phi)*cos(psy) + cos(phi)*sin(theta)*sin(psy),cos(phi)*cos(theta),0),
							vec4(0,0,0,1)
							);

	mat4 sizeMatrix = mat4(
						vec4(scale.x,0,0,0),
						vec4(0,scale.y,0,0),
						vec4(0,0,scale.z,0),
						vec4(0,0,0,1)
						);

	transformMatrix =  translationMatrix * rotationMatrix * sizeMatrix;
}

vec3 Transform::LocalDirectionToWorld(vec3 direction)
{
	vec4 hDirection = vec4(direction.x,direction.y,direction.z,0.f);
	hDirection = transformMatrix * hDirection;
	return vec3(hDirection.x,hDirection.y,hDirection.z);
}

vec3 Transform::LocalPositionToWorld(vec3 direction)
{
	vec4 hDirection = vec4(direction.x,direction.y,direction.z,1.f);
	hDirection = transformMatrix * hDirection;
	return vec3(hDirection.x,hDirection.y,hDirection.z);
}
