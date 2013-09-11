#include "RigidBody.h"


RigidBody::RigidBody    (Transform* p):
	m_forcesVector      (vector<Force>()),
	m_torquesVector     (vector<Force>()),
	m_acceleration      (vec3(0)),
	m_velocity          (vec3(0)),
	m_angularVelocity   (vec3(0)),
	m_previousPosition  (vec3(0)),
	m_previousRotation  (vec3(0))
{
	int extern fps;
	m_transform = p;
	deltaTime = 1.f/fps;
	frictionCoefficient = 0.05f;
	mass = 1.f;
}


RigidBody::~RigidBody(void)
{
}

// Public:

void RigidBody::Update()
{
	UpdateAcceleration();
	UpdateVelocity();
	UpdateTransform();
}

void RigidBody::PushForce(vec3 vector,RigidBody::Force::ForceMode mode)
{
	Force forcePushed(vector, mode);
	m_forcesVector.push_back(forcePushed);
}

void RigidBody::PushTorque(vec3 vector,RigidBody::Force::ForceMode mode)
{
	Force torquePushed(vector, mode);
	m_torquesVector.push_back(torquePushed);
}

void RigidBody::SetPosition(vec3 newPosition)
{
	// Don't loose your speed friend
	m_previousPosition = newPosition - m_velocity;
	m_transform->position = newPosition;
}

void RigidBody::SetRotation(vec3 newRotation)
{
	// don't loose it either
	m_previousRotation = newRotation - m_angularVelocity;
	m_transform->rotation = newRotation;
}


// Private:

void RigidBody::UpdateAcceleration()
{
	vec3 newAcceleration = vec3(0);
	vector<Force> newVector = vector<Force>();

	for(uint i=0; i<m_forcesVector.size(); i++)
	{
		Force currentForce = m_forcesVector.at(i);

		if(currentForce.m_mode == Force::ForceMode::Impulse)
		{
			newAcceleration += currentForce.m_force / mass;
		}
		else if(currentForce.m_mode == Force::ForceMode::Constant)
		{
			newAcceleration += currentForce.m_force / mass;
			newVector.push_back(currentForce);
		}
	}
	m_forcesVector = newVector;
	m_acceleration = newAcceleration;

	vec3 newAngular = vec3(0);
	vector<Force> newAngularVector = vector<Force>();

	for(uint i=0; i<m_torquesVector.size(); i++)
	{
		Force currentForce = m_torquesVector.at(i);

		if(currentForce.m_mode == Force::ForceMode::Impulse)
		{
			newAngular += currentForce.m_force / mass;
		}
		else if(currentForce.m_mode == Force::ForceMode::Constant)
		{
			newAcceleration += currentForce.m_force / mass;
			newAngularVector.push_back(currentForce);
		}
	}
	m_torquesVector = newAngularVector;
	m_angularAcceleration = newAngular;
}

void RigidBody::UpdateVelocity()
{
	vec3 position = m_transform->position;
	vec3 newVelocity = position - m_previousPosition;
	m_previousPosition = position;

	vec3 rotation = m_transform->rotation;
	vec3 newAngularVelocity = rotation - m_previousRotation;
	m_previousRotation = rotation;

	m_velocity = newVelocity;
	m_angularVelocity = newAngularVelocity;
}

void RigidBody::UpdateTransform()
{
	vec3 friction = -frictionCoefficient*m_velocity;
	//	P = dP + d2P - fdP
	m_transform->position += m_velocity + m_acceleration*deltaTime*deltaTime + friction;

	vec3 angularFriction = -frictionCoefficient*m_angularVelocity;
	m_transform->rotation += m_angularVelocity + m_angularAcceleration*deltaTime*deltaTime + angularFriction;
}