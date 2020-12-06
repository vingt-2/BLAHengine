// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Maths/Maths.h"

namespace BLA
{
    class CollisionProcessor;
    class Timer;
    class RigidBodyComponent;

    class SoftBodyDefinition
    {
        
    };

    class SoftBodySystem
    {
    public:
        int m_substeps;
        float m_uniformViscosity;
        blaVec3 m_gravity;
        bool m_enableGravity;
        bool m_tieToTime;

        SoftBodySystem(Timer* time);
        ~SoftBodySystem();

        void UpdateSystem() {};
        
        float GetTimeStep() { return m_timeStep; };

        void SetTimeObject(Timer* time);

    private:
        blaVector<SoftBodyDefinition> m_softBodies;
        Timer* m_time;

        float m_timeStep;
        float m_oldTime;

        void ApplyWorldForces();
        void GetNewStates();
        void UpdateStates();
    };
}