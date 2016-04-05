#pragma once
#include "../../../Std/Maths.h"
#include "../../../Std/std.h"
#include "Collider.h"

class RigidBody
{
public:
	vec3 m_previousPosition;
	vec3 m_previousRotation;
	vec3 m_acceleration;
	vec3 m_velocity;
	vec3 m_angularAcceleration;
	vec3 m_angularVelocity;

	float m_mass;
	mat3 m_inertiaTensor;
	mat3 m_invInertiaTensor;

	bool m_isPinned;
	bool m_applyGravity;

	Transform* m_transform;
	Collider* m_collider;

	void Update();

	void AddForce(vec3 vector);
	void AddTorque(vec3 vector);
	void SetPosition(vec3 Position);
	void SetRotation(vec3 rotation);
	vec3 GetForcesAccu() { return m_forcesAccu; };
	vec3 GetTorquesAccu() { return m_torquesAccu; };
	void ClearForces() { m_forcesAccu = vec3(0); m_torquesAccu = vec3(0); }

	RigidBody(Transform* transform, MeshRenderer* mesh);
	~RigidBody(void);

private:

	vec3 m_forcesAccu;
	vec3 m_torquesAccu;

	vec3 m_linearImpulseAccu;
	vec3 m_angularImpulseAccu;
};
