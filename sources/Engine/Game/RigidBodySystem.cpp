#include "RigidBodySystem.h"

RigidBodySystem::RigidBodySystem():
	m_timeStep(0.01f),
	m_uniformFriction(0.05f),
	m_gravity(vec3(0, -10, 0))
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
	body.m_acceleration = (body.GetForcesAccu() / body.m_mass);
	body.m_angularAcceleration = (body.GetTorquesAccu() / body.m_mass);
	body.ClearForces();
}

void RigidBodySystem::UpdateVelocity(RigidBody& body)
{
	Transform* transform = body.m_transform;
	vec3 position = transform->position;
	vec3 newVelocity = position - body.m_previousPosition;
	body.m_previousPosition = position;

	vec3 rotation = transform->rotation;
	vec3 newAngularVelocity = rotation - body.m_previousRotation;
	body.m_previousRotation = rotation;

	body.m_velocity = newVelocity;
	body.m_angularVelocity = newAngularVelocity;
}

/*
	Forward Euler solver
*/
void RigidBodySystem::UpdateTransform(RigidBody& body)
{
	Transform* transform = body.m_transform;
	vec3 friction = -m_uniformFriction*body.m_velocity;

	transform->position += body.m_velocity + body.m_acceleration*m_timeStep*m_timeStep + friction;

	vec3 angularFriction = -m_uniformFriction*body.m_angularVelocity;
	transform->rotation += body.m_angularVelocity + body.m_angularAcceleration*m_timeStep*m_timeStep + angularFriction;
}

void RigidBodySystem::ApplyWorldForces()
{
	for (auto body : m_rigidBodyList)
	{
		if (body->m_applyGravity)
			body->AddForce(m_gravity);
	}
}