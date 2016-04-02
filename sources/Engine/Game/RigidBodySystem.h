#include "../../Std/std.h"
#include "GameComponents\RigidBody.h"

#pragma once
class RigidBodySystem
{
public:
	float m_uniformFriction;

	RigidBodySystem();
	~RigidBodySystem();

	int RegisterRigidBody(RigidBody &body);
	float GetTimeStep() { return m_timeStep; };

	void SolveSystem();

private:
	vector<RigidBody*> m_rigidBodyList;
	float m_timeStep;

	void UpdateAcceleration(RigidBody& body);
	void UpdateVelocity(RigidBody& body);
	void UpdateTransform(RigidBody& body);
};

