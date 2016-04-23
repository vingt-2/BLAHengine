#include "RigidBodySystem.h"

RigidBodySystem::RigidBodySystem() :
	m_timeStep(0.001f),
	m_uniformFriction(0.01),
	m_gravity(vec3(0, -1, 0)),
	m_substeps(1),
	m_oldTime(-1),
	m_isSimulating(false),
	m_enableGravity(true)
{
	m_collisionProcessor = new CollisionProcessor();

	m_timeStep /= sqrt(m_substeps);
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

void RigidBodySystem::EnableSimulation()
{
	if (!m_isSimulating)
	{
		m_oldTime = glfwGetTime();
		m_isSimulating = true;
	}
}

void RigidBodySystem::DisableSimulation()
{
	m_isSimulating = false;
}

void RigidBodySystem::UpdateSystem()
{
	if (m_isSimulating)
	{
		double time = glfwGetTime();
		double timeStep = 2*(time - m_oldTime) / 1;
		if (timeStep < 0.1)
			m_timeStep = timeStep;

		m_oldTime = time;

		ApplyWorldForces();
		for (int i = 0; i < m_substeps; i++)
		{
			GetNewStates();
			m_collisionProcessor->ProcessCollisions();
			UpdateStates();
		}
	}
}

void RigidBodySystem::UpdateStates()
{
	for (RigidBody* bodyPtr : (m_rigidBodyList))
	{
		RigidBody& body = *bodyPtr;
		if (!body.m_isPinned)
		{
			if (body.m_nextState == NULL) cout << "Empty state for body, check your calls!\n";
			else UpdateTransform(body);
		}
	}
}

void RigidBodySystem::GetNewStates()
{
	for (RigidBody* bodyPtr : (m_rigidBodyList))
	{
		RigidBody& body = *bodyPtr;
		if (!body.m_isPinned)
		{
			UpdateAcceleration(body);
			UpdateVelocity(body);
		}
		else
		{
			body.m_nextState = new NextState();
			body.m_invInertiaTensor = mat3(0);
			body.m_invMassTensor = mat3(0);
			body.ClearForces();
		}
	}
}

void RigidBodySystem::UpdateAcceleration(RigidBody& body)
{
	NextState* newState = new NextState();

	mat3 omegaHat = matrixCross(body.m_angularVelocity);

	newState->m_acceleration = body.m_invMassTensor * ((body.GetForcesAccu()));
	
	vec3 inertia = omegaHat * body.m_inertiaTensor * body.m_angularVelocity;
	newState->m_angularAcceleration = -body.m_invInertiaTensor * (body.GetTorquesAccu() - inertia);
	
	body.m_nextState = newState;
}

/*
	Forward Euler Velocity Step
*/
void RigidBodySystem::UpdateVelocity(RigidBody& body)
{
	if (body.m_nextState == NULL) 
	{
		cout << "Next State not available, creating it, but you should call UpdateAcceleration First.\n";
		UpdateAcceleration(body);
	}

	body.m_nextState->m_velocity = body.m_velocity + (m_timeStep * body.m_nextState->m_acceleration);
	body.m_nextState->m_angularVelocity = body.m_angularVelocity + (m_timeStep * body.m_nextState->m_angularAcceleration);
}

/*
	Forward Euler Transform Update
*/
void RigidBodySystem::UpdateTransform(RigidBody& body)
{
	Transform* transform = body.m_transform;
	
	//	Update State, delete NextState entry;
	{
		vec3 correctionL;
		vec3 correctionA;
		if (m_collisionProcessor->debug_stop)
		{
			correctionL = vec3(0);
			correctionA = vec3(0);
		}
		else
		{
			correctionL = body.m_nextState->m_correctionLinearVelocity;
			correctionA = body.m_nextState->m_correctionAngularVelocity;
		}

		body.m_acceleration = body.m_nextState->m_acceleration;
		body.m_angularAcceleration = body.m_nextState->m_angularAcceleration;
		body.m_velocity = body.m_nextState->m_velocity + correctionL;

		body.m_angularVelocity = body.m_nextState->m_angularVelocity + transform->WorldDirectionToLocal(correctionA);

		// Debug
		body.m_debugCorrectionVelocity = correctionA;
		delete(body.m_nextState);
		body.m_nextState = NULL;
		body.ClearForces();
	}

	//	Evaluate new Position;
	transform->m_position += m_timeStep * body.m_velocity;
	
	//	Evaluate Exponential Map
	mat3 omegaHat = matrixCross(-body.m_angularVelocity);
	mat3 deltaRot = mat3(1) + sin(m_timeStep) * omegaHat + (1 - cos(m_timeStep)) * (omegaHat*omegaHat);
	mat3 newRotation = transform->m_rotation *= deltaRot;

	//	Normalize each axis of rotation to avoid scale drift 
	vec3 X = vec3(newRotation[0][0], newRotation[0][1], newRotation[0][2]);
	vec3 Y = vec3(newRotation[1][0], newRotation[1][1], newRotation[1][2]);
	vec3 Z = vec3(newRotation[2][0], newRotation[2][1], newRotation[2][2]);
	
	transform->m_rotation = mat3(normalize(X), normalize(Y), normalize(Z));
}

void RigidBodySystem::ApplyWorldForces()
{
	for (auto body : m_rigidBodyList)
	{
		vec3 linearFriction = -1.f * m_uniformFriction * body->m_velocity;
		vec3 angularFriction = -1.f * m_uniformFriction * body->m_angularVelocity;
		body->AddLinearForce(linearFriction);
		body->AddTorque(angularFriction);
		if (body->m_applyGravity && m_enableGravity)
		{
			body->AddLinearForce(m_gravity);
		}
	}
}