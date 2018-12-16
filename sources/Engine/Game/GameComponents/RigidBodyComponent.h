#pragma once
#include "../../../Common/Maths/Maths.h"
#include "../../../Common/StdInclude.h"
#include "ColliderComponent.h"

namespace BLAengine
{

    class BLACORE_API NextState
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

    class BLACORE_API RigidBodyComponent : public GameComponent
    {
    public:
        blaVec3 m_previousPosition;
        blaVec3 m_previousRotation;
        blaVec3 m_acceleration;
        blaVec3 m_velocity;
        blaVec3 m_angularAcceleration;
        blaVec3 m_angularVelocity;

        blaVec3 m_debugCorrectionVelocity;

        // Pack the next state if needed to avoid recomputing
        NextState* m_nextState;

        blaMat3 m_massTensor;
        blaMat3 m_invMassTensor;
        blaMat3 m_inertiaTensor;
        blaMat3 m_invInertiaTensor;

        bool m_isPinned;
        bool m_applyGravity;
        bool m_enableCollision;

        void SetCollider(ColliderComponent* collider);

        void CreateAndSetMeshCollider(TriangleMesh* mesh);

        ColliderComponent* GetAssociatedCollider() const { return m_associatedCollider; }

        void Update();

        void PushForceWorld(blaVec3 pushAtW, blaVec3 forceW);

        void AddLinearForce(blaVec3 force);
        void AddTorque(blaVec3 vector);
        void AddImpulse(blaVec3 impulse);

        blaVec3 GetForcesAccu() const { return m_forcesAccu; };
        blaVec3 GetTorquesAccu() const { return m_torquesAccu; };
        void ClearForces() { m_forcesAccu = blaVec3(0); m_torquesAccu = blaVec3(0); }

        RigidBodyComponent(GameObject* parentObject);
        ~RigidBodyComponent(void);

    private:

        blaVec3 m_forcesAccu;
        blaVec3 m_torquesAccu;
        blaVec3 m_impulseAccu;

        blaVec3 m_linearImpulseAccu;
        blaVec3 m_angularImpulseAccu;

        ColliderComponent* m_associatedCollider;
    };

}