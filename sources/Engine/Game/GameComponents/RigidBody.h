#pragma once
#include "../../../Std/Maths.h"
#include "../../../Std/std.h"
#include "Collider.h"

class NextState 
{
public:
	NextState();

	vec3 m_acceleration;
	vec3 m_velocity;
	vec3 m_angularAcceleration;
	vec3 m_angularVelocity;
	vec3 m_correctionLinearVelocity;
	vec3 m_correctionAngularVelocity;
};

class RigidBody
{
public:
	vec3 m_previousPosition;
	vec3 m_previousRotation;
	vec3 m_acceleration;
	vec3 m_velocity;
	vec3 m_angularAcceleration;
	vec3 m_angularVelocity;

	vec3 m_debugCorrectionVelocity;

	// Pack the next state if needed to avoid recomputing
	NextState* m_nextState;

	mat3 m_massTensor;
	mat3 m_invMassTensor;
	mat3 m_inertiaTensor;
	mat3 m_invInertiaTensor;

	bool m_isPinned;
	bool m_applyGravity;

	Transform* m_transform;
	Collider* m_collider;

	void Update();

	void PushForceWorld(vec3 pushAt, vec3 forceInWorld);

	void AddLinearForce(vec3 force);
	void AddTorque(vec3 vector);
	void AddImpulse(vec3 impulse);

	vec3 GetForcesAccu() { return m_forcesAccu; };
	vec3 GetTorquesAccu() { return m_torquesAccu; };
	void ClearForces() { m_forcesAccu = vec3(0); m_torquesAccu = vec3(0); }

	RigidBody(Transform* transform, MeshRenderer* mesh);
	~RigidBody(void);

private:

	vec3 m_forcesAccu;
	vec3 m_torquesAccu;
	vec3 m_impulseAccu;

	vec3 m_linearImpulseAccu;
	vec3 m_angularImpulseAccu;
};
