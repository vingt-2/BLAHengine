#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>
#include <Engine/Core/GameComponent.h>
#include <Engine/Core/Scene.h>
#include <Engine/EngineInstance.h>
#include <Engine/System/Console.h>
#include <Engine/Physics/RigidBodyComponent.h>
#include <Engine/Core/DebugDraw.h>
#include <Common/DataStructures/CircularBuffer.h>

#include <random>

#pragma optimize("", off)

namespace BLAengine
{
    BLA_CONSOLE_VAR(BoidStiffness, float, 20.f)
    BLA_CONSOLE_VAR(InterBoidStiffness, float, 0.05f)
    BLA_CONSOLE_VAR(InterBoidDistance, float, 100.f)
    BLA_CONSOLE_VAR(BoidDamping, float, 3.f)

    BEGIN_COMPONENT_DECLARATION(BoidComponent)

        void Update() override;

        BoidComponent(GameObjectReference parentObject) : GameComponent(parentObject) {}
        ~BoidComponent(void) override {}

        GameObjectReference m_target;
        blaVec3 m_color;
        blaF32 m_hominStiffnessMult;
        blaF32 m_interBoidStiffnessMult;
        blaF32 m_distanceMult;
        int m_framesSinceLastPush = 0;

    private:
        blaVector<GameObjectReference> m_otherBoids;
        CircularBuffer<blaVec3, 100> m_pastPositions;

    END_DECLARATION()

    BEGIN_COMPONENT_DESCRIPTION(BoidComponent)
        EXPOSE(m_target)
        EXPOSE(m_color)
    END_DESCRIPTION()

    void BoidComponent::Update()
    {
        if(!m_target.IsValid()) 
        {
            m_target = EngineInstance::GetSingletonInstance()->GetWorkingScene()->FindObjectByName("BoidTarget");
        }

        RigidBodyComponent* rigidBody = GetOwnerObject()->GetComponent<RigidBodyComponent>();

        if (!rigidBody) return;

        if (m_target.IsValid()) 
        {
            rigidBody->AddLinearForce(g_BoidStiffness *  m_hominStiffnessMult * (m_target->GetTransform().GetPosition() - GetObjectTransform().GetPosition()) - g_BoidDamping * rigidBody->m_velocity);
        }

        if(m_otherBoids.empty()) 
        {
            for (GameObjectReference& obj : EngineInstance::GetSingletonInstance()->GetWorkingScene()->GetObjects()) 
            {
                if (BoidComponent* boid = obj->GetComponent<BoidComponent>())
                    m_otherBoids.push_back(obj);
            }
        }

        for(GameObjectReference& boid : m_otherBoids) 
        {
            blaVec3 toBoid = boid->GetTransform().GetPosition() - GetObjectTransform().GetPosition();

            blaF32 toBoidL = glm::length(toBoid) + 0.001f;

            rigidBody->AddLinearForce(g_InterBoidStiffness * m_interBoidStiffnessMult * (toBoidL - m_distanceMult * g_InterBoidDistance) / toBoidL * toBoid);
        }

        DebugDraw::DrawSphere(GetObjectTransform().GetPosition(), 0.2f, blaVec4(m_color, 1.f), 4);

        m_pastPositions.Push(GetObjectTransform().GetPosition());

        int size = m_pastPositions.GetCount();
        for (int i = 0; i < size - 1; i++)
        {
            DebugDraw::DrawLine(m_pastPositions.Peek(i), m_pastPositions.Peek(i + 1), m_color);
        }
    }

    BLA_CONSOLE_COMMAND(int, CreateBoidSystem, int numberOfBoids)
    {
        Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();

        scene->CreateObject("BoidTarget");

        std::random_device rd2;
        std::mt19937 dgen(rd2());
        std::uniform_real_distribution<float> dist(0, 1);
        std::normal_distribution<float> normalDist(0.7f, 0.3f);

        for (int i = 0; i < numberOfBoids; i++)
        {
            GameObjectReference ref = scene->CreateObject("Boid" + std::to_string(i));
            BoidComponent* boidComponent = static_cast<BoidComponent*>(GameComponentManager::GetSingletonInstance()->CreateComponent("BoidComponent", ref));
            GameComponentManager::GetSingletonInstance()->CreateComponent("RigidBodyComponent", ref);

            boidComponent->m_color = blaVec3(dist(dgen), dist(dgen), dist(dgen));
            boidComponent->m_hominStiffnessMult = dist(dgen);
            boidComponent->m_interBoidStiffnessMult = dist(dgen);
            boidComponent->m_distanceMult = normalDist(dgen);

            ref->GetTransform().SetPosition(blaVec3(dist(dgen), dist(dgen), dist(dgen)));
        }

        return 0;
    }
}
