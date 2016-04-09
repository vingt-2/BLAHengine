#include "Transform.h"


Transform::Transform(void):
	m_position(vec3(0)), m_rotation(mat3(1)), m_scale(vec3(1))
{
}


Transform::~Transform(void)
{}

void Transform::UpdateTransform()
{
	mat4 sizeMatrix = mat4
	(
		vec4(m_scale.x, 0, 0, 0),
		vec4(0, m_scale.y, 0, 0),
		vec4(0, 0, m_scale.z, 0),
		vec4(0, 0, 0, 1)
	);

	m_transformMatrix = mat4
	(
		vec4(m_rotation[0][0], m_rotation[0][1], m_rotation[0][2], 0),
		vec4(m_rotation[1][0], m_rotation[1][1], m_rotation[1][2], 0),
		vec4(m_rotation[2][0], m_rotation[2][1], m_rotation[2][2], 0),
		vec4(m_position.x, m_position.y, m_position.z, 1)
	);

	m_transformMatrix = m_transformMatrix * sizeMatrix;
}

void Transform::SetRotationUsingEuler(vec3 eulerAngles)
{

	float phi = eulerAngles.x;
	float theta = eulerAngles.y;
	float psy = eulerAngles.z;

	m_rotation = mat3
	(
		vec3(cos(theta)*cos(psy), -cos(theta)*sin(psy), sin(theta)),
		vec3(cos(phi)*sin(psy) + sin(phi)*sin(theta)*cos(psy), cos(phi)*cos(psy) - sin(phi)*sin(theta)*sin(psy), -sin(phi)*cos(theta)),
		vec3(sin(phi)*sin(psy) - cos(phi)*sin(theta)*cos(psy), sin(phi)*cos(psy) + cos(phi)*sin(theta)*sin(psy), cos(phi)*cos(theta))
	);
}

vec3 Transform::LocalDirectionToWorld(vec3 direction)
{
	vec4 hDirection = vec4(direction.x,direction.y,direction.z,0.f);
	hDirection = m_transformMatrix * hDirection;
	return vec3(hDirection.x,hDirection.y,hDirection.z);
}

vec3 Transform::LocalPositionToWorld(vec3 direction)
{
	vec4 hDirection = vec4(direction.x,direction.y,direction.z,1.f);
	hDirection = m_transformMatrix * hDirection;
	return vec3(hDirection.x,hDirection.y,hDirection.z);
}

vec3 Transform::WorldDirectionToLocal(vec3 direction)
{
	vec4 hDirection = vec4(direction.x,direction.y,direction.z,0.f);
	hDirection = inverse(m_transformMatrix) * hDirection;
	return vec3(hDirection.x,hDirection.y,hDirection.z);
}

vec3 Transform::WorldlPositionToLocal(vec3 direction)
{
	vec4 hDirection = vec4(direction.x,direction.y,direction.z,1.f);
	hDirection = inverse(m_transformMatrix) * hDirection;
	return vec3(hDirection.x,hDirection.y,hDirection.z);
}


