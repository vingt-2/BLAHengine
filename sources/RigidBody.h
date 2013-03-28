#pragma once
#include "Graphics.h"
#include "std.h"
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

		ForceMode mode;
		vec3 force;

		Force(vec3 newForce,ForceMode newMode)
		{
			force = newForce;
			mode = newMode;
		}
		~Force()
		{}
	};

	float mass;
	float frictionCoefficient;

	float deltaTime;

	Transform* transform;

	void Update();

	void PushForce(Force force);
	void PushTorque(Force torque);
	void SetPosition(vec3 Position);

	RigidBody(Transform* parent);
	~RigidBody(void);

private:

	vec3 acceleration;
	vec3 velocity;
	vec3 angularAcceleration;
	vec3 angularVelocity;

	vec3 previousPosition;
	vec3 previousRotation;

	vector<Force> forcesVector;
	vector<Force> torqueVector;

	void UpdateAcceleration();
	void UpdateVelocity();
	void UpdateTransform();

};
