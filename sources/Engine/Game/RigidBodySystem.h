#pragma once
#include "../../Common/StdInclude.h"
#include "./CollisionProcessing/CollisionProcessor.h"

namespace BLAengine
{
    class BLACORE_API RigidBodySystem
    {
    public:
        int m_substeps;
        float m_uniformViscosity;
        blaVec3 m_gravity;
        bool m_enableGravity;
        bool m_tieToTime;

        RigidBodySystem(Timer* time);
        ~RigidBodySystem();

        void UpdateSystem();
        bool RegisterRigidBody(RigidBodyComponent &body);
        float GetTimeStep() { return m_timeStep; };

        void EnableSimulation();
        void DisableSimulation();

        CollisionProcessor* m_collisionProcessor;

        void SetTimeObject(Timer* time);


    private:
        vector<RigidBodyComponent*> m_rigidBodyList;
        Timer* m_time;

        float m_timeStep;
        float m_oldTime;
        bool m_isSimulating;

        void ApplyWorldForces();
        void GetNewStates();
        void UpdateStates();

        void UpdateAcceleration(RigidBodyComponent& body);
        void UpdateVelocity(RigidBodyComponent& body);
        void UpdateTransform(RigidBodyComponent& body);
    };
}