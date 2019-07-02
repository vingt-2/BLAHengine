#pragma once

#include <Common/Maths/Maths.h>
#include <Common/StdInclude.h>
#include "ColliderComponent.h"

#define HAIR_SEGMENT_COUNT 50
#define HAIR_LENGTH 0.2f

namespace BLAengine
{
    // Should make a big matrix and store everything more contiguous but whatever ...
    struct HairStrand
    {
        blaVec3 m_attachementPointLocal;
        float m_segmentLength;
        float m_stiffness;
        blaVec3 m_particlePositionsW[HAIR_SEGMENT_COUNT + 1];
        blaVec3 m_particleVelocities[HAIR_SEGMENT_COUNT + 1];
    };

    class BLACORE_API SimpleHairComponent : public GameComponent
    {
    public:

        bool m_isPinned;
        bool m_applyGravity;

        void Update();

        void PushForceWorld(blaVec3 pushAtW, blaVec3 forceW);
        void AddLinearForce(blaVec3 force);

        blaVec3 GetForcesAccu() { return m_forcesAccu; };
        void ClearForces() { m_forcesAccu = blaVec3(0); }
        void ClearImpulse() { m_impulseAccu = blaVec3(0); }
        void AddHairStrand(blaVec3 positionW);

        SimpleHairComponent(GameObjectReference parentObject);
        ~SimpleHairComponent(void) override;

    private:

        float m_timestep;

        void RenderSimpleHair();

        void StepHair();

        blaVector<HairStrand> m_hairStrands;

        blaVec3 m_forcesAccu;
        blaVec3 m_impulseAccu;
    };

}