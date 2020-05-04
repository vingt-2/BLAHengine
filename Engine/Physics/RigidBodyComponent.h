#pragma once
#include "Maths/Maths.h"
#include "StdInclude.h"

#include "Core/BehaviorComponent.h"

namespace BLA
{
    class TriangleMesh;
    class ColliderComponent;
    class NextState
    {
    public:
        NextState();

        blaVec3 m_acceleration;
        blaVec3 m_velocity;
        blaVec3 m_angularAcceleration;
        blaVec3 m_angularVelocity;

        blaVec3 m_nextPos;

        // Using blaVec3 for LCP solver values
        blaVec3 m_correctionLinearVelocity;
        blaVec3 m_correctionAngularVelocity;
    };

    BeginBehaviorDeclaration(BLAEngine, RigidBodyComponent)
    public:
        blaVec3 m_previousPosition;
        blaVec3 m_previousRotation;
        blaVec3 m_acceleration;
        blaVec3 m_velocity;
        blaVec3 m_angularAcceleration;
        blaVec3 m_angularVelocity;

        blaVec3 m_debugCorrectionVelocity;

        // Pack the next state if needed to avoid recomputing
        NextState m_nextState;

        blaMat3 m_massTensor = blaMat3(1);
        blaMat3 m_invMassTensor = blaMat3(1);
        blaMat3 m_inertiaTensor;
        blaMat3 m_invInertiaTensor;

        bool m_isPinned = false;
        bool m_applyGravity = true;
        bool m_enableCollision = true;

        BLA_EXPORT(BLAEngine) void SetCollider(ColliderComponent* collider);

        BLA_EXPORT(BLAEngine)void CreateAndSetMeshCollider(TriangleMesh* mesh);

        BLA_EXPORT(BLAEngine) ColliderComponent* GetAssociatedCollider() const;

        BLA_EXPORT(BLAEngine) void Init() override;

        BLA_EXPORT(BLAEngine) void PushForceWorld(blaVec3 pushAtW, blaVec3 forceW);

        BLA_EXPORT(BLAEngine) void AddLinearForce(blaVec3 force);
        BLA_EXPORT(BLAEngine) void AddTorque(blaVec3 vector);
        BLA_EXPORT(BLAEngine) void AddImpulse(blaVec3 impulse);

        BLA_EXPORT(BLAEngine) blaVec3 GetForcesAccu() const;;
        BLA_EXPORT(BLAEngine) blaVec3 GetTorquesAccu() const;;
        BLA_EXPORT(BLAEngine) blaVec3 GetImpulseAccu() const;
        BLA_EXPORT(BLAEngine) void ClearForces();
        BLA_EXPORT(BLAEngine) void ClearImpulse();

    private:

        blaVec3 m_forcesAccu;
        blaVec3 m_torquesAccu;
        blaVec3 m_impulseAccu;

        blaVec3 m_linearImpulseAccu;
        blaVec3 m_angularImpulseAccu;

        ColliderComponent* m_associatedCollider;

    EndBehaviorDeclaration()
}