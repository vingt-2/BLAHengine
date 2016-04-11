#include "RigidBody.h"


RigidBody::RigidBody(Transform* transform, MeshRenderer* mesh) :
	m_forcesAccu(vec3(0)),
	m_torquesAccu(vec3(0)),
	m_acceleration(vec3(0)),
	m_velocity(vec3(0)),
	m_angularVelocity(vec3(0)),
	m_previousPosition(vec3(0)),
	m_previousRotation(vec3(0)),
	m_isPinned(false),
	m_applyGravity(true),
	m_impulseAccu(0),
	m_massTensor(mat3(1)),
	m_inertiaTensor(mat3(1))
{
	m_transform = transform;

	if (mesh == NULL) 
		m_collider = NULL;
	else 
		m_collider = new Collider(mesh);

	m_invMassTensor = inverse(m_massTensor);
	m_invInertiaTensor = inverse(m_inertiaTensor);
}


RigidBody::~RigidBody(void)
{}

void RigidBody::Update()
{}

void RigidBody::PushForceWorld(vec3 pushAt, vec3 forceInWorld)
{
	vec3 contactInBody = pushAt - m_transform->m_position;

	vec3 torque = cross(forceInWorld, contactInBody);
	torque = m_transform->WorldDirectionToLocal(torque);

	AddTorque(torque);
	AddLinearForce(forceInWorld / length(torque));
}

void RigidBody::AddLinearForce(vec3 force)
{
	m_forcesAccu += force;
}

void RigidBody::AddTorque(vec3 torque)
{
	m_torquesAccu += torque;
}

void RigidBody::AddImpulse(vec3 impulse)
{
	m_impulseAccu += impulse;
}

NextState::NextState():
	m_acceleration(vec3(0)),
	m_velocity(vec3(0)),
	m_angularAcceleration(vec3(0)),
	m_angularVelocity(vec3(0)),
	m_correctionLinearVelocity(vec3(0)),
	m_correctionAngularVelocity(vec3(0))
{}
