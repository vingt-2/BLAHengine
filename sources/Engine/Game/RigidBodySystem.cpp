#include "RigidBodySystem.h"

RigidBodySystem::RigidBodySystem():
	m_timeStep(0.05f),
	m_uniformFriction(0.005f),
	m_gravity(vec3(0, 0, 0))
{
	m_collisionProcessor = new CollisionProcessor();
}


RigidBodySystem::~RigidBodySystem()
{
}

int RigidBodySystem::RegisterRigidBody(RigidBody &body)
{
	m_rigidBodyList.push_back(&body);
	
	if (body.m_collider)
		m_collisionProcessor->m_bodiesList.push_back(&body);

	return 0;
}

void RigidBodySystem::UpdateSystem()
{
	ApplyWorldForces();
	m_collisionProcessor->ProcessCollisions();
	SolveSystem();
}

void RigidBodySystem::SolveSystem()
{
	for (RigidBody* bodyPtr : (m_rigidBodyList))
	{
		RigidBody& body = *bodyPtr;
		if (!body.m_isPinned)
		{
			UpdateAcceleration(body);
			UpdateVelocity(body);
			UpdateTransform(body);
		}
	}
}

void RigidBodySystem::UpdateAcceleration(RigidBody& body)
{
	vec3 linearFriction = m_uniformFriction * body.m_velocity;
	vec3 angularFriction = m_uniformFriction * body.m_angularVelocity;

	body.m_acceleration = (body.GetForcesAccu() - linearFriction / body.m_mass);
	body.m_angularAcceleration = (body.GetTorquesAccu() - angularFriction / body.m_mass);
	
	body.ClearForces();
}

/*
	Forward Euler Velocity Step
*/
void RigidBodySystem::UpdateVelocity(RigidBody& body)
{
	body.m_velocity = m_timeStep * body.m_acceleration;
	body.m_angularVelocity += m_timeStep * body.m_angularAcceleration;
}

/*
	Forward Euler Transform Update
*/
void RigidBodySystem::UpdateTransform(RigidBody& body)
{
	Transform* transform = body.m_transform;
	
	transform->position += m_timeStep * body.m_velocity;
	transform->rotation += m_timeStep * body.m_angularVelocity;
}

void RigidBodySystem::ApplyWorldForces()
{
	for (auto body : m_rigidBodyList)
	{
		if (body->m_applyGravity)
			body->AddForce(m_gravity);
	}
}