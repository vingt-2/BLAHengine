#pragma once
#include "../../Common/StdInclude.h"
#include "./CollisionProcessing/CollisionProcessor.h"

class BLACORE_API RigidBodySystem
{
public:
	int m_substeps;
	float m_uniformViscosity;
	vec3 m_gravity;
	bool m_enableGravity;
	bool m_tieToTime;

	RigidBodySystem(Time* time);
	~RigidBodySystem();

	void UpdateSystem();
	bool RegisterRigidBody(RigidBody &body);
	float GetTimeStep() { return m_timeStep; };

	void EnableSimulation();
	void DisableSimulation();

	CollisionProcessor* m_collisionProcessor;

private:
	vector<RigidBody*> m_rigidBodyList;
	Time* m_time;

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

