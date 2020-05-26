// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "Core/TransformComponent.h"

#include "Core/GameObject.h"

using namespace BLA;

BeginBehaviorDescription(RigidBodyComponent, Dependencies(RootSystem))
Expose(m_applyGravity)
Expose(m_velocity)
EndBehaviorDescription()

void RigidBodyComponent::SetCollider(ColliderComponent* collider)
{
    if (collider->GetOwnerObject() != GetOwnerObject())
    {
        //TODO: Say something sassy about the programmer that programmed that
        return;
    }

    m_associatedCollider = collider;
}

void RigidBodyComponent::CreateAndSetMeshCollider(TriangleMesh* mesh)
{
    MeshCollider* newMeshColliderComponent = nullptr;// BLA_CREATE_COMPONENT(MeshCollider, GetOwnerObject());
    newMeshColliderComponent->SetColliderMesh(mesh);
    // m_associatedCollider = newMeshColliderComponent;
}

ColliderComponent* RigidBodyComponent::GetAssociatedCollider() const
{
    return m_associatedCollider;
}

void RigidBodyComponent::Init()
{
    m_invMassTensor = inverse(m_massTensor);
    m_invInertiaTensor = inverse(m_inertiaTensor);
}

void RigidBodyComponent::Update()
{
    // TODO: Eventually do some stuff here ?
}

void RigidBodyComponent::PushForceWorld(blaVec3 pushAtW, blaVec3 forceW)
{
    blaScaledTransform transform = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
    blaVec3 contactInBody = pushAtW - transform.GetPosition();

    blaVec3 torque = cross(forceW, contactInBody);
    torque = transform.WorldDirectionToLocal(torque);

    AddTorque(torque);

    blaF32 torqueMagnitude = length(torque);

    if (torqueMagnitude > BLA_EPSILON)
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

blaVec3 RigidBodyComponent::GetForcesAccu() const
{
    return m_forcesAccu;
}

blaVec3 RigidBodyComponent::GetTorquesAccu() const
{
    return m_torquesAccu;
}

blaVec3 RigidBodyComponent::GetImpulseAccu() const
{
    return m_impulseAccu;
}

void RigidBodyComponent::ClearForces()
{
    m_forcesAccu = blaVec3(0);
    m_torquesAccu = blaVec3(0);
}

void RigidBodyComponent::ClearImpulse()
{
    m_impulseAccu = blaVec3(0);
}

NextState::NextState() :
    m_acceleration(blaVec3(0)),
    m_velocity(blaVec3(0)),
    m_angularAcceleration(blaVec3(0)),
    m_angularVelocity(blaVec3(0)),
    m_correctionLinearVelocity(blaVec3(0)),
    m_correctionAngularVelocity(blaVec3(0))
{}
