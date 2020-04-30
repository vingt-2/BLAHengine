#pragma once
#include "StdInclude.h"
#include "Maths/Maths.h"
#include "Core/ComponentSystems.h"

#include "Physics/ColliderComponent.h"
#include "Physics/RigidBodyComponent.h"

namespace BLA
{
    class CollisionProcessor;
    class Timer;
    class RigidBodyComponent;

    BeginComponentSystemDeclaration(BLAEngine, RigidBodySystem, InputComponents(ColliderComponent), OutputComponents(RigidBodyComponent))
    public:
        // Todo autogenerate constructor declaration in macro
        RigidBodySystem(blaStringId name, blaVector<blaStringId> systemDependencies);
        ~RigidBodySystem() {}

        int m_substeps;
        float m_uniformViscosity;
        blaVec3 m_gravity;
        bool m_enableGravity;
        bool m_tieToTime;

        void UpdateSystem();
        float GetTimeStep() { return m_timeStep; };

        void EnableSimulation();
        void DisableSimulation();

        CollisionProcessor* m_collisionProcessor;

        void SetTimeObject(Timer* time);


    private:
        blaVector<RigidBodyComponent*> m_rigidBodyList;
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
    EndComponentSystemDeclaration()
}