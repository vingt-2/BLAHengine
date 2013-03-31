#include "RigidBody.h"


RigidBody::RigidBody(Transform* p):
	forcesVector(vector<Force>()),
	torqueVector(vector<Force>()),
	acceleration(vec3(0)),
	velocity(vec3(0)),
	angularVelocity(vec3(0)),
	previousPosition(vec3(0)),
	previousRotation(vec3(0))
{
	int extern fps;
	transform = p;
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
	forcesVector.push_back(forcePushed);
}

void RigidBody::PushTorque(vec3 vector,RigidBody::Force::ForceMode mode)
{
	Force torquePushed(vector, mode);
	torqueVector.push_back(torquePushed);
}

void RigidBody::SetPosition(vec3 newPosition)
{
	// Don't loose your speed friend
	previousPosition = newPosition - velocity;
	transform->position = newPosition;
}


// Private:

void RigidBody::UpdateAcceleration()
{
	vec3 newAcceleration = vec3(0);
	vector<Force> newVector = vector<Force>();

	for(uint i=0; i<forcesVector.size(); i++)
	{
		Force currentForce = forcesVector.at(i);

		if(currentForce.mode == Force::ForceMode::Impulse)
		{
			newAcceleration += currentForce.force / mass;
		}
		else if(currentForce.mode == Force::ForceMode::Constant)
		{
			newAcceleration += currentForce.force / mass;
			newVector.push_back(currentForce);
		}
	}
	forcesVector = newVector;
	acceleration = newAcceleration;

	vec3 newAngular = vec3(0);
	vector<Force> newAngularVector = vector<Force>();

	for(uint i=0; i<torqueVector.size(); i++)
	{
		Force currentForce = torqueVector.at(i);

		if(currentForce.mode == Force::ForceMode::Impulse)
		{
			newAngular += currentForce.force / mass;
		}
		else if(currentForce.mode == Force::ForceMode::Constant)
		{
			newAcceleration += currentForce.force / mass;
			newAngularVector.push_back(currentForce);
		}
	}
	torqueVector = newAngularVector;
	angularAcceleration = newAngular;
}

void RigidBody::UpdateVelocity()
{
	vec3 position = transform->position;
	vec3 newVelocity = position - previousPosition;
	previousPosition = position;

	vec3 rotation = transform->rotation;
	vec3 newAngularVelocity = rotation - previousRotation;
	previousRotation = rotation;

	velocity = newVelocity;
	angularVelocity = newAngularVelocity;
}

void RigidBody::UpdateTransform()
{
	vec3 friction = -frictionCoefficient*velocity;
	//	P = dP + d2P - fdP
	transform->position += velocity + acceleration*deltaTime*deltaTime + friction;

	vec3 angularFriction = -frictionCoefficient*angularVelocity;
	transform->rotation += angularVelocity + angularAcceleration*deltaTime*deltaTime + angularFriction;
}