#pragma once
#include <StdInclude.h>
#include <Maths/Maths.h>
#include <Core/Scene.h>
#include <EngineInstance.h>
#include <System/Console.h>
#include <Physics/RigidBodyComponent.h>
#include <Core/TransformComponent.h>
#include <Core/DebugDraw.h>
#include <DataStructures/CircularBuffer.h>

#include <random>

namespace BLAengine
{
    BLA_CONSOLE_VAR(float, BoidStiffness, 20.f)
    BLA_CONSOLE_VAR(float, InterBoidStiffness, 0.05f)
    BLA_CONSOLE_VAR(float, InterBoidDistance, 100.f)
    BLA_CONSOLE_VAR(float, BoidDamping, 3.f)

    BEGIN_COMPONENT_DECLARATION(DemosLibrary, BoidComponent)

        void Update() override;

        GameObject m_target;
        blaVec3 m_color;
        blaF32 m_hominStiffnessMult;
        blaF32 m_interBoidStiffnessMult;
        blaF32 m_distanceMult;
        int m_framesSinceLastPush = 0;

    private:
        blaVector<GameObject> m_otherBoids;
        CircularBuffer<blaVec3, 20> m_pastPositions;

    END_DECLARATION()

    BEGIN_COMPONENT_DESCRIPTION(BoidComponent)
        EXPOSE(m_target)
        EXPOSE(m_color)
    END_DESCRIPTION()

	std::random_device g_rd2;
	std::mt19937 g_dgen(g_rd2());

    void BoidComponent::Update()
    {
        if(!m_target.IsValid()) 
        {
            m_target = EngineInstance::GetSingletonInstance()->GetWorkingScene()->FindObjectByName("BoidTarget");
        }

        RigidBodyComponent* rigidBody = GetOwnerObject().GetComponent<RigidBodyComponent>();

        if (!rigidBody) return;

        blaVec3 myPosition = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetPosition();

        if (m_target.IsValid()) 
        {
            TransformComponent* tComp = m_target.GetComponent<TransformComponent>();

            rigidBody->AddLinearForce(g_BoidStiffness *  m_hominStiffnessMult * (tComp->GetTransform().GetPosition() - myPosition) - g_BoidDamping * rigidBody->m_velocity);
        }

        if(m_otherBoids.empty()) 
        {
            for (GameObject& obj : EngineInstance::GetSingletonInstance()->GetWorkingScene()->GetObjects()) 
            {
                if (BoidComponent* boid = obj.GetComponent<BoidComponent>())
                    m_otherBoids.push_back(obj);
            }
        }

		std::uniform_int_distribution<> dist(0, m_otherBoids.size());

		int slice = dist(g_dgen);

        for(size_t i = slice; i < m_otherBoids.size() && i < slice + 300; i++) 
        {
			GameObject& boid = m_otherBoids[i];
            TransformComponent* tComp = boid.GetComponent<TransformComponent>();

            blaVec3 toBoid = tComp->GetTransform().GetPosition() - myPosition;

            blaF32 toBoidL = glm::length(toBoid) + 0.001f;

            rigidBody->AddLinearForce(g_InterBoidStiffness * m_interBoidStiffnessMult * (toBoidL - m_distanceMult * g_InterBoidDistance) / toBoidL * toBoid);
        }

        DebugDraw::DrawSphere(myPosition, 0.2f, blaVec4(m_color, 1.f), 4);

        m_pastPositions.Push(myPosition);

        int size = m_pastPositions.GetCount();
        for (int i = 0; i < size - 1; i++)
        {
            //DebugDraw::DrawLine(m_pastPositions.Peek(i), m_pastPositions.Peek(i + 1), m_color);
        }
    }

    BLA_CONSOLE_COMMAND(int, CreateBoidSystem, int numberOfBoids)
    {
        Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();

        scene->CreateObject(BlaStringId("BoidTarget"));

        std::uniform_real_distribution<float> dist(0.2f, 1.f);
        std::normal_distribution<float> normalDist(0.7f, 0.3f);

        for (int i = 0; i < numberOfBoids; i++)
        {
            GameObject ref = scene->CreateObject(GenerateBlaStringId("Boid" + std::to_string(i))); // TODO: Please don't ...

            BoidComponent* boidComponent = ref.CreateComponent<BoidComponent>();
            ref.CreateComponent<RigidBodyComponent>();

            boidComponent->m_color = blaVec3(dist(g_dgen), dist(g_dgen), dist(g_dgen));
            boidComponent->m_hominStiffnessMult = dist(g_dgen);
            boidComponent->m_interBoidStiffnessMult = dist(g_dgen);
            boidComponent->m_distanceMult = normalDist(g_dgen);

            blaScaledTransform s(blaVec3(1.f), blaPosQuat(blaVec3(dist(g_dgen), dist(g_dgen), dist(g_dgen)), blaPosQuat::QuatIdentity()));

            ref.GetComponent<TransformComponent>()->SetTransform(s);
        }

        return 0;
    }
}
