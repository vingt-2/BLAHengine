#pragma once
#include "../../../Std/Maths.h"
#include "../../../Std/std.h"
#include "Transform.h"

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

	Transform* m_transform;

	void Update();

	void AddForce(vec3 vector);
	void AddTorque(vec3 vector);
	void SetPosition(vec3 Position);
	void SetRotation(vec3 rotation);
	vec3 GetForcesAccu() { return m_forcesAccu; };
	vec3 GetTorquesAccu() { return m_torquesAccu; };
	void ClearForces() { m_forcesAccu = vec3(0); m_torquesAccu = vec3(0); }

	RigidBody(Transform* parent);
	~RigidBody(void);

private:

	vec3 m_forcesAccu;
	vec3 m_torquesAccu;

	vec3 m_linearImpulseAccu;
	vec3 m_angularImpulseAccu;
};
