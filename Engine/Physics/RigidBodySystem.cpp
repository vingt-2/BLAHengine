// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Physics/CollisionProcessing/CollisionProcessor.h"
#include "Physics/RigidBodyComponent.h"
#include "Core/TransformComponent.h"
#include "Core/GameObject.h"
#include "Core/Timer.h"

#include "RigidBodySystem.h"

using namespace BLA;
using namespace BLA::Core;

RegisterComponentSystem(RigidBodySystem, Dependencies(RootSystem))

RigidBodySystem::RigidBodySystem(blaStringId name, blaVector<blaStringId> systemDependencies) :
TypedComponentSystem<IOTS<ColliderComponent>, IOTS<RigidBodyComponent>>(name, systemDependencies),
    m_timeStep(0.01f),
    m_uniformViscosity(0.01f),
    m_gravity(blaVec3(0.f, -5.f, 0.f)),
    m_substeps(1),
    m_oldTime(-1),
    m_isSimulating(true),
    m_enableGravity(true),
    m_tieToTime(true)
{
    m_collisionProcessor = new CollisionProcessor(m_time, &m_timeStep);
    m_timeStep /= sqrtf((float)m_substeps);
}

template <>
void TypedComponentSystem<IOTS<ColliderComponent>, IOTS<RigidBodyComponent>>::
Execute(SystemObjectsIterator& systemObjects, InputComponents<ColliderComponent>, OutputComponents<RigidBodyComponent>)
{
    
}


//bool RigidBodySystem::RegisterRigidBody(RigidBodyComponent &body)
//{
//    m_rigidBodyList.push_back(&body);
//    body.m_registered = true;
//    if (body.GetAssociatedCollider())
//    {
//        m_collisionProcessor->m_bodiesList.push_back(&body);
//        body.m_registered = true;
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}

void RigidBodySystem::EnableSimulation()
{
    if (!m_isSimulating)
    {
        m_oldTime = m_time->GetTime();
        m_isSimulating = true;
    }
}

void RigidBodySystem::DisableSimulation()
{
    m_isSimulating = false;
}

void RigidBodySystem::SetTimeObject(Timer * time)
{
    m_collisionProcessor->setTimeObject(time);
    m_time = time;
    m_oldTime = m_time->GetTime();
}

void RigidBodySystem::UpdateSystem()
{
    if (m_isSimulating)
    {
        float time = m_time->GetTime();
        if (m_tieToTime)
        {
            float timeStep = 2.f * (time - m_oldTime) / 1.f;
            if (timeStep < 0.1f)
                m_timeStep = timeStep;

            if (m_timeStep == 0)
                m_timeStep = 0.01f;

        }
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
    for (RigidBodyComponent* bodyPtr : (m_rigidBodyList))
    {
        RigidBodyComponent& body = *bodyPtr;
        if (!body.m_isPinned)
        {
            UpdateTransform(body);
        }
    }
}

void RigidBodySystem::GetNewStates()
{
    for (RigidBodyComponent* bodyPtr : (m_rigidBodyList))
    {
        RigidBodyComponent& body = *bodyPtr;

        blaScaledTransform bodyTransform = body.GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();

        if (!body.m_isPinned)
        {
            UpdateAcceleration(body);
            UpdateVelocity(body);

            body.m_nextState.m_nextPos = bodyTransform.GetPosition() + m_timeStep * body.m_velocity;
        }
        else
        {
            body.m_nextState = NextState();
            body.m_invInertiaTensor = blaMat3(0);
            body.m_invMassTensor = blaMat3(0);
            body.ClearForces();
            body.ClearImpulse();
        }
    }
}

void RigidBodySystem::UpdateAcceleration(RigidBodyComponent& body)
{
    NextState newState;

    blaMat3 omegaHat = matrixCross(body.m_angularVelocity);

    newState.m_acceleration = body.m_invMassTensor * ((body.GetForcesAccu()));

    blaVec3 inertia = omegaHat * body.m_inertiaTensor * body.m_angularVelocity;
    newState.m_angularAcceleration = -body.m_invInertiaTensor * (body.GetTorquesAccu() - inertia);

    body.m_nextState = newState;
}

/*
    Forward Euler Velocity Step
*/
void RigidBodySystem::UpdateVelocity(RigidBodyComponent& body)
{
    //if (body.m_nextState== nullptr) 
    //{
    //    cout << "Next State not available, creating it, but you should call UpdateAcceleration First.\n";
    //    UpdateAcceleration(body);
    //}

    body.m_nextState.m_velocity = body.m_velocity + (m_timeStep * body.m_nextState.m_acceleration) + body.GetImpulseAccu();
    body.m_nextState.m_angularVelocity = body.m_angularVelocity + (m_timeStep * body.m_nextState.m_angularAcceleration);
}

/*
    Forward Euler TransformComponent Update
*/
void RigidBodySystem::UpdateTransform(RigidBodyComponent& body)
{
    TransformComponent* transformComponent = body.GetOwnerObject().GetComponent<TransformComponent>();
    blaScaledTransform bodyTransform = transformComponent->GetTransform();

    //    Update State, delete NextState entry;
    {
        blaVec3 correctionL;
        blaVec3 correctionA;
        if (m_collisionProcessor->debug_stop)
        {
            correctionL = blaVec3(0);
            correctionA = blaVec3(0);
        }
        else
        {
            correctionL = body.m_nextState.m_correctionLinearVelocity;
            correctionA = body.m_nextState.m_correctionAngularVelocity;
        }

        body.m_acceleration = body.m_nextState.m_acceleration;
        body.m_angularAcceleration = body.m_nextState.m_angularAcceleration;
        body.m_velocity = body.m_nextState.m_velocity + correctionL;

        body.m_angularVelocity = body.m_nextState.m_angularVelocity + bodyTransform.WorldDirectionToLocal(correctionA);

        // Debug
        body.m_debugCorrectionVelocity = correctionA;

        body.ClearForces();
        body.ClearImpulse();
    }

    //    Evaluate new Position;
    blaVec3 newPos = bodyTransform.GetPosition() + m_timeStep * body.m_velocity;
    bodyTransform.SetPosition(newPos);

    //    Evaluate Exponential Map
    blaMat3 omegaHat = matrixCross(-body.m_angularVelocity);
    blaMat3 deltaRot = blaMat3(1) + sin(m_timeStep) * omegaHat + (1 - cos(m_timeStep)) * (omegaHat*omegaHat);
    blaMat3 newRotation = toMat3(bodyTransform.GetRotation()) * deltaRot;

    ////    Normalize each axis of rotation to avoid scale drift 
    //blaVec3 X = blaVec3(newRotation[0][0], newRotation[0][1], newRotation[0][2]);
    //blaVec3 Y = blaVec3(newRotation[1][0], newRotation[1][1], newRotation[1][2]);
    //blaVec3 Z = blaVec3(newRotation[2][0], newRotation[2][1], newRotation[2][2]);


    //// TODO: Implement conversion in blaQuat class ...
    //glm::quat q = blaMat3(normalize(X), normalize(Y), normalize(Z));
    //transform.SetRotation(blaQuat(q.x,q.y,q.z,q.w));

    blaVec3 logMapAxis = SafeNormalize(body.m_angularVelocity);
    blaF32 logMapAngle = glm::length(body.m_angularVelocity) * m_timeStep;

    bodyTransform.SetRotation(glm::angleAxis(logMapAngle, logMapAxis));

    transformComponent->SetTransform(bodyTransform);
}

void RigidBodySystem::ApplyWorldForces()
{
    for (auto body : m_rigidBodyList)
    {
        blaVec3 linearFriction = -1.f * m_uniformViscosity * body->m_velocity;
        blaVec3 angularFriction = 1.f * m_uniformViscosity * body->m_angularVelocity;
        body->AddLinearForce(linearFriction);
        body->AddTorque(angularFriction);
        if (body->m_applyGravity && m_enableGravity)
        {
            body->AddLinearForce(m_gravity);
        }
    }
}