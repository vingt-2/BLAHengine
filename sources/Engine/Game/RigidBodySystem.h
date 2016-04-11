#include "../../Std/std.h"
#include "CollisionProcessor.h"

#pragma once
class RigidBodySystem
{
public:
	float m_uniformFriction;
	vec3 m_gravity;

	RigidBodySystem();
	~RigidBodySystem();

	void UpdateSystem();
	int RegisterRigidBody(RigidBody &body);
	float GetTimeStep() { return m_timeStep; };

	CollisionProcessor* m_collisionProcessor;

private:
	vector<RigidBody*> m_rigidBodyList;
	float m_timeStep;

	void ApplyWorldForces();
	void GetNewStates();
	void UpdateStates();
	
	void UpdateAcceleration(RigidBody& body);
	void UpdateVelocity(RigidBody& body);
	void UpdateTransform(RigidBody& body);
};

