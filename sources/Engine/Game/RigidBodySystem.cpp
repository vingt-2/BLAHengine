#include "RigidBodySystem.h"

RigidBodySystem::RigidBodySystem():
	m_timeStep(0.05f),
	m_uniformFriction(0.05f),
	m_gravity(vec3(0, -0, 0))
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

	body.m_acceleration = ((body.GetForcesAccu() - linearFriction) / body.m_mass);// -matrixCross(body.m_angularVelocity)*body.m_velocity;
	body.m_angularAcceleration = (body.GetTorquesAccu() - angularFriction);
	
	body.ClearForces();
}

/*
	Forward Euler Velocity Step
*/
void RigidBodySystem::UpdateVelocity(RigidBody& body)
{
	body.m_velocity += m_timeStep * body.m_acceleration;
	body.m_angularVelocity += m_timeStep * body.m_angularAcceleration;
}

/*
	Forward Euler Transform Update
*/
void RigidBodySystem::UpdateTransform(RigidBody& body)
{
	Transform* transform = body.m_transform;
	
	transform->m_position += m_timeStep * body.m_velocity;

	/*
		Evaluate Exponential Map
	*/

	mat3 omegaHat = matrixCross(body.m_angularVelocity);
	mat3 deltaRot = mat3(1) + sin(m_timeStep) * omegaHat + (1 - cos(m_timeStep)) * (omegaHat*omegaHat);
	mat3 newRotation = transform->m_rotation *= deltaRot;

	/*
		Normalize each axis of rotation to avoid scale drift
	*/

	vec3 X = vec3(newRotation[0][0], newRotation[0][1], newRotation[0][2]);
	vec3 Y = vec3(newRotation[1][0], newRotation[1][1], newRotation[1][2]);
	vec3 Z = vec3(newRotation[2][0], newRotation[2][1], newRotation[2][2]);
	
	transform->m_rotation = mat3(normalize(X), normalize(Y), normalize(Z));
}

void RigidBodySystem::ApplyWorldForces()
{
	for (auto body : m_rigidBodyList)
	{
		if (body->m_applyGravity)
			body->AddLinearForce(m_gravity);
	}
}