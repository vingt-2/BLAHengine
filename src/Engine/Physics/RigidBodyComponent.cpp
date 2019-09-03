#include "RigidBodyComponent.h"
#include "ColliderComponent.h"

#include <Engine/Core/GameObject.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(RigidBodyComponent)
END_DESCRIPTION()

RigidBodyComponent::RigidBodyComponent(GameObjectReference parentObject) :
    GameComponent(parentObject),
    m_forcesAccu(blaVec3(0)),
    m_torquesAccu(blaVec3(0)),
    m_acceleration(blaVec3(0)),
    m_velocity(blaVec3(0)),
    m_angularVelocity(blaVec3(0)),
    m_previousPosition(blaVec3(0)),
    m_previousRotation(blaVec3(0)),
    m_isPinned(false),
    m_applyGravity(true),
    m_impulseAccu(0),
    m_massTensor(blaMat3(1)),
    m_inertiaTensor(blaMat3(1)),
    m_enableCollision(true)
{
    m_associatedCollider = nullptr;

    m_invMassTensor = inverse(m_massTensor);
    m_invInertiaTensor = inverse(m_inertiaTensor);
}


RigidBodyComponent::~RigidBodyComponent(void)
{}

void RigidBodyComponent::SetCollider(ColliderComponent* collider)
{
    if (collider->GetParentObject() != GetParentObject())
    {
        //TODO: Say something sassy about the programmer that programmed that
        return;
    }
    
    m_associatedCollider = collider;
}

void RigidBodyComponent::CreateAndSetMeshCollider(TriangleMesh* mesh)
{
    MeshColliderComponent* newMeshColliderComponent = BLA_CREATE_COMPONENT(MeshColliderComponent, GetParentObject());
    newMeshColliderComponent->SetColliderMesh(mesh);
    m_associatedCollider = newMeshColliderComponent;
}

void RigidBodyComponent::Update()
{
// TODO: Eventually do some stuff here ?
}

void RigidBodyComponent::PushForceWorld(blaVec3 pushAtW, blaVec3 forceW)
{
    ObjectTransform transform = GetParentObject()->GetTransform();
    blaVec3 contactInBody = pushAtW - transform.GetPosition();

    blaVec3 torque = cross(forceW, contactInBody);
    torque = transform.WorldDirectionToLocal(torque);

    AddTorque(torque);

	blaF32 torqueMagnitude = length(torque);

	if(torqueMagnitude > BLA_EPSILON)
	{
		AddLinearForce(forceW / length(torque));
	}
	else
	{
		AddLinearForce(forceW);
	}
}

void RigidBodyComponent::AddLinearForce(blaVec3 force)
{
    m_forcesAccu += force;
}

void RigidBodyComponent::AddTorque(blaVec3 torque)
{
    m_torquesAccu += torque;
}

void RigidBodyComponent::AddImpulse(blaVec3 impulse)
{
    m_impulseAccu += impulse;
}

NextState::NextState():
    m_acceleration(blaVec3(0)),
    m_velocity(blaVec3(0)),
    m_angularAcceleration(blaVec3(0)),
    m_angularVelocity(blaVec3(0)),
    m_correctionLinearVelocity(blaVec3(0)),
    m_correctionAngularVelocity(blaVec3(0))
{}
