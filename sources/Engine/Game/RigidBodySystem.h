#include "../../Std/std.h"
#include "CollisionProcessor.h"

#pragma once
class RigidBodySystem
{
public:
	int m_substeps;
	float m_uniformFriction;
	vec3 m_gravity;
	bool m_enableGravity;

	RigidBodySystem();
	~RigidBodySystem();

	void UpdateSystem();
	int RegisterRigidBody(RigidBody &body);
	float GetTimeStep() { return m_timeStep; };

	void EnableSimulation();
	void DisableSimulation();

	CollisionProcessor* m_collisionProcessor;

private:
	vector<RigidBody*> m_rigidBodyList;
	
	float m_timeStep;
	double m_oldTime;
	bool m_isSimulating;

	void ApplyWorldForces();
	void GetNewStates();
	void UpdateStates();
	
	void UpdateAcceleration(RigidBody& body);
	void UpdateVelocity(RigidBody& body);
	void UpdateTransform(RigidBody& body);
};

