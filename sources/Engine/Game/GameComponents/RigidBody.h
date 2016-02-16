#pragma once
#include "../../../Std/Maths.h"
#include "../../../Std/std.h"
#include "Transform.h"

class RigidBody
{
public:

	class Force
	{
	public:
		enum ForceMode
		{
			Impulse,
			Constant
		};

		ForceMode m_mode;
		vec3 m_force;

		Force(vec3 newForce,ForceMode newMode)
		{
			m_force = newForce;
			m_mode = newMode;
		}
		~Force()
		{}
	};

	float m_mass;
	float m_frictionCoefficient;

	float m_deltaTime;

	Transform* m_transform;

	void Update();

	void PushForce(vec3 vector,RigidBody::Force::ForceMode mode);
	void PushTorque(vec3 vector,RigidBody::Force::ForceMode mode);
	void SetPosition(vec3 Position);
	void SetRotation(vec3 rotation);

	RigidBody(Transform* parent);
	~RigidBody(void);

private:

	vec3 m_acceleration;
	vec3 m_velocity;
	vec3 m_angularAcceleration;
	vec3 m_angularVelocity;

	vec3 m_previousPosition;
	vec3 m_previousRotation;

	vector<Force> m_forcesVector;
	vector<Force> m_torquesVector;

	void UpdateAcceleration();
	void UpdateVelocity();
	void UpdateTransform();

};
