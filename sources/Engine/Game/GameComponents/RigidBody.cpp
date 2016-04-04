#include "RigidBody.h"


RigidBody::RigidBody    (Transform* transform, MeshRenderer* mesh):
	m_forcesAccu        (vec3(0)),
	m_torquesAccu       (vec3(0)),
	m_acceleration      (vec3(0)),
	m_velocity          (vec3(0)),
	m_angularVelocity   (vec3(0)),
	m_previousPosition  (vec3(0)),
	m_previousRotation  (vec3(0))
{
	m_transform = transform;

	if (mesh == NULL) 
		m_collider = NULL;
	else 
		m_collider = new Collider(mesh);

	m_mass = 1.f;
}


RigidBody::~RigidBody(void)
{
}

void RigidBody::Update()
{
}

void RigidBody::AddForce(vec3 force)
{
	m_forcesAccu += force;
}

void RigidBody::AddTorque(vec3 torque)
{
	m_torquesAccu += torque;
}

void RigidBody::SetPosition(vec3 newPosition)
{
	// Don't loose your speed friend
	m_previousPosition = newPosition - m_velocity;
	m_transform->position = newPosition;
}

void RigidBody::SetRotation(vec3 newRotation)
{
	// don't loose it either
	m_previousRotation = newRotation - m_angularVelocity;
	m_transform->rotation = newRotation;
}