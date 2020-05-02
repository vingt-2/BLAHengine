#pragma once

#include "Maths/Maths.h"
#include "StdInclude.h"
#include "ColliderComponent.h"

#define HAIR_SEGMENT_COUNT 50
#define HAIR_LENGTH 0.2f

namespace BLA
{
    // Should make a big matrix and store everything more contiguous 
    struct HairStrand
    {
        blaVec3 m_attachementPointLocal;
        float m_segmentLength;
        float m_stiffness;
        blaVec3 m_particlePositionsW[HAIR_SEGMENT_COUNT + 1];
        blaVec3 m_particleVelocities[HAIR_SEGMENT_COUNT + 1];
    };

    BeginComponentDeclaration(BLAEngine, SimpleHairComponent)
        bool m_isPinned = false;
        bool m_applyGravity = true;
		bool m_renderDebug = false;

        void Update();

        void PushForceWorld(blaVec3 pushAtW, blaVec3 forceW);
        void AddLinearForce(blaVec3 force);

        blaVec3 GetForcesAccu() { return m_forcesAccu; };
        void ClearForces() { m_forcesAccu = blaVec3(0); }
        void ClearImpulse() { m_impulseAccu = blaVec3(0); }
        void AddHairStrand(blaVec3 positionW);

    private:

        float m_timestep = 1.0f / 500.f;

        void DebugRenderSimpleHair();

        void StepHair();

        blaVector<HairStrand> m_hairStrands;

        blaVec3 m_forcesAccu = blaVec3(0);
        blaVec3 m_impulseAccu = blaVec3(0);
    EndComponentDeclaration()
}