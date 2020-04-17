#include "SimpleHairComponent.h"
#include "EngineInstance.h"
#include "Core/GameObject.h"
#include "Core/DebugDraw.h"
#include "Core/TransformComponent.h"

#include <random>

using namespace BLAengine;

std::default_random_engine g_random_generator;

BeginComponentDescription(SimpleHairComponent)
Expose(m_isPinned)
Expose(m_applyGravity)
Expose(m_renderDebug)
EndComponentDescription()

typedef struct sphere_t
{
    blaVec3 origin;
    float radius;
} sphere;

bool collideWithSphere(sphere sphere, blaVec3 position, blaVec3 &requiredCorrection)
{
    float epsilon = 0.1f;
    blaVec3 toSphere = position - sphere.origin;
    float l = length(toSphere);
    if (l < sphere.radius)
    {
        requiredCorrection = (sphere.radius - l) * toSphere / l;

        return true;
    }

    return false;
}

void SimpleHairComponent::DebugRenderSimpleHair()
{
    const EngineInstance* engineInstance = EngineInstance::GetSingletonInstanceRead();

    for (auto hair : m_hairStrands)
    {
        for (int i = 0; i < HAIR_SEGMENT_COUNT; i++)
        {
            blaVec3 color = blaVec3(0.f);

            DebugDraw::DrawLine(hair.m_particlePositionsW[i], hair.m_particlePositionsW[i + 1], color);
        }
    }
}

void SimpleHairComponent::StepHair()
{
    blaScaledTransform transform = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
    sphere s;
    s.origin = transform.GetPosition();
    s.radius = 1.05f; // Add an Epsilon to avoid z-figthing, and give stifness to the first segments...

    for (size_t h = 0; h < m_hairStrands.size(); h++)
    {
        HairStrand* hair = &m_hairStrands[h];
        // Update particle 0
        blaVec3 newP0Position = transform.LocalPositionToWorld(hair->m_attachementPointLocal);
        blaVec3 newP0Velocity = (newP0Position - hair->m_particlePositionsW[0]) / m_timestep;
        hair->m_particlePositionsW[0] = newP0Position;

        //hair->m_particleVelocities[0] = newP0Velocity;

        blaVec3 correctionDisplacement[HAIR_SEGMENT_COUNT + 1];
        correctionDisplacement[0] = blaVec3(0.f);

        for (int i = 1; i < HAIR_SEGMENT_COUNT + 1; i++)
        {
            // Add collisions ...
            blaVec3 repulsion;
            if (collideWithSphere(s, hair->m_particlePositionsW[i], repulsion))
            {
                DebugDraw::GetSingletonInstance()->DrawLine(hair->m_particlePositionsW[i], hair->m_particlePositionsW[i] + repulsion, blaVec3(1.f, 0.f, 0.f));
                hair->m_particleVelocities[i] += 0.60f * repulsion / m_timestep;
            }

            // Add additional stiffness...
            blaVec3 parentToPos = normalize(hair->m_particlePositionsW[1] - hair->m_particlePositionsW[0]);
            hair->m_particleVelocities[i] += (float)exp(-0.05f * (float)i) * hair->m_stiffness * parentToPos;
            //g_debugInstance->DrawLine(hair->m_particlePositionsW[i], hair->m_particlePositionsW[i] + hair->m_particleVelocities[i] , blaVec3(1.f, 0.f, 0.f));

            /*float stifnessCoeff = 10.0f;
            blaVec3 parentToPos = normalize(hair->m_particlePositionsW[1] - hair->m_particlePositionsW[0]);
            hair->m_particleVelocities[i] += (float)exp(-0.0f5f * (float)i) *stifnessCoeff * parentToPos;*/


            blaVec3 newPosition = hair->m_particlePositionsW[i] + m_timestep * hair->m_particleVelocities[i] + m_timestep * P_GRAVITY;

            blaVec3 parentToNewPos = normalize(newPosition - hair->m_particlePositionsW[i - 1]);
            blaVec3 projectedNewPos = hair->m_particlePositionsW[i - 1] + parentToNewPos * hair->m_segmentLength;

            correctionDisplacement[i] = projectedNewPos - newPosition;

            hair->m_particleVelocities[i] = (projectedNewPos - hair->m_particlePositionsW[i]) / m_timestep;

            hair->m_particlePositionsW[i] = projectedNewPos;
        }

        for (int i = 1; i < HAIR_SEGMENT_COUNT; i++)
        {
            hair->m_particleVelocities[i] -= 0.90f * correctionDisplacement[i + 1] / m_timestep;
        }
    }
}

void SimpleHairComponent::Update()
{
    StepHair();

	if(m_renderDebug)
	{
		DebugRenderSimpleHair();
	}
}

void SimpleHairComponent::PushForceWorld(blaVec3 pushAtW, blaVec3 forceW)
{
    blaScaledTransform transform = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();

    blaVec3 contactInBody = pushAtW - transform.GetPosition();

    blaVec3 torque = cross(forceW, contactInBody);
    torque = transform.WorldDirectionToLocal(torque);

    AddLinearForce(forceW / length(torque));
}

void SimpleHairComponent::AddLinearForce(blaVec3 force)
{
    m_forcesAccu += force;
}

void SimpleHairComponent::AddHairStrand(blaVec3 positionW)
{
    blaScaledTransform transform = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();

    HairStrand hairStrand;
    hairStrand.m_attachementPointLocal = transform.WorldPositionToLocal(positionW);

    blaVec3 upDir = blaVec3(0.f, 1.f, 0.f);

    std::uniform_real_distribution<float> g_float_distribution(0.2f, 1.f);

    for (int i = 0; i < HAIR_SEGMENT_COUNT + 1; i++) // n+1 particles
    {
        const float hairLength = g_float_distribution(g_random_generator);
        const float stiffness = 10.f * g_float_distribution(g_random_generator);
        const float segmentLength = hairLength / (float)HAIR_SEGMENT_COUNT;

        hairStrand.m_segmentLength = segmentLength;
        hairStrand.m_stiffness = stiffness;
        hairStrand.m_particlePositionsW[i] = positionW + i * segmentLength * upDir;
        hairStrand.m_particleVelocities[i] = blaVec3(0.f, 0.f, 0.f);
    }

    m_hairStrands.push_back(hairStrand);
}