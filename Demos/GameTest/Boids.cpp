#pragma once

#include <StdInclude.h>
#include "Maths/Maths.h"
#include "Core/Scene.h"
#include "EngineInstance.h"
#include "System/Console.h"
#include "Physics/RigidBodyComponent.h"
#include "Core/TransformComponent.h"
#include "Core/DebugDraw.h"
#include "DataStructures/CircularBuffer.h"
#include "Core/BehaviorComponent.h"

#include <random>
#include "Geometry/PrimitiveGeometry.h"
#include "Renderer/MeshRendererComponent.h"
#include "Assets/AssetsManager.h"

namespace BLA
{
    DeclareConsoleVariable(float, BoidStiffness, 20.f);
    DeclareConsoleVariable(float, InterBoidStiffness, 0.05f);
    DeclareConsoleVariable(float, InterBoidDistance, 100.f);
    DeclareConsoleVariable(float, BoidDamping, 3.f);

    BeginComponentDeclaration(BLADemos, ParticleComponent)
        void Init() override { m_velocity = blaVec3(0); m_acceleration = blaVec3(0); m_mass = 1.f; }
        blaVec3 m_velocity, m_acceleration;
        blaF32 m_mass;
        void AddForce(blaVec3 force) { m_acceleration += force / m_mass; }
    EndComponentDeclaration()

    BeginComponentDescription(ParticleComponent)
        Expose(m_velocity)
        Expose(m_acceleration)
    EndComponentDescription()

    DeclareComponentSystem(BLADemos, ParticleSystem, InputComponents(), OutputComponents(TransformComponent, ParticleComponent));

    RegisterComponentSystem(ParticleSystem, Dependencies(IntegrateBoidSystem))

    template <>
    void ParticleSystem::Execute(SystemObjectsIterator& systemObjects, OutputComponents<TransformComponent> ioTransform, OutputComponents<ParticleComponent> ioParticles)
    {
        float dt = Timer::GetSingletonInstanceRead()->GetDelta();
        for(auto o : systemObjects)
        {
            ParticleComponent& particle = *ioParticles[o];
            TransformComponent& t = *ioTransform[o];
            particle.m_velocity += particle.m_acceleration * dt;
            blaScaledTransform tr = t.GetTransform();
            tr.SetPosition(tr.GetPosition() + particle.m_velocity * dt);
            ioTransform[o]->SetTransform(tr);
            particle.m_acceleration = blaVec3(0);
        }
    }

    // Target should be system unique for example ... To store in a system state ?
    BeginComponentDeclaration(BLADemos, BoidComponent)
        GameObject m_target;
        blaVec3 m_color;
        blaF32 m_hominStiffnessMult;
        blaF32 m_interBoidStiffnessMult;
        blaF32 m_distanceMult;
        int m_framesSinceLastPush = 0;

        CircularBuffer<blaVec3, 20> m_pastPositions;
    EndComponentDeclaration()

    BeginComponentDescription(BoidComponent)
        Expose(m_target)
        Expose(m_color)
    EndComponentDescription()

    DeclareComponentSystem(BLADemos, IntegrateBoidSystem, InputComponents(BoidComponent, TransformComponent), OutputComponents(ParticleComponent));
    RegisterComponentSystem(IntegrateBoidSystem, Dependencies(RootSystem));

    std::random_device g_rd2;
    std::mt19937 g_dgen(g_rd2());

    template <>
    void IntegrateBoidSystem::Execute(SystemObjectsIterator& systemObjects, InputComponents<BoidComponent> iBoids, InputComponents<TransformComponent> iTransforms, OutputComponents<ParticleComponent> oParticle)
    {
        for (const GameObjectID& systemObject : systemObjects)
        {
            ParticleComponent* particle = oParticle[systemObject];
            const TransformComponent* transform = iTransforms[systemObject];
            const BoidComponent* boidComponent = iBoids[systemObject];

            blaVec3 myPosition = transform->GetLocalTransform().GetPosition();

            GameObject target = boidComponent->m_target;

            if (target.IsValid())
            {
                const TransformComponent* targetTransform = iTransforms[target];

                particle->AddForce(g_BoidStiffness * boidComponent->m_hominStiffnessMult * (targetTransform->GetLocalTransform().GetPosition() - myPosition) - g_BoidDamping * particle->m_velocity);
            }

            std::uniform_int_distribution<> dist(0, systemObjects.size());

            int slice = dist(g_dgen);

            for (size_t i = slice; i < systemObjects.size() && i < slice + 3; i++)
            {
                const TransformComponent* tComp = iTransforms[systemObjects[i]];

                blaVec3 toBoid = tComp->GetLocalTransform().GetPosition() - myPosition;

                blaF32 toBoidL = glm::length(toBoid) + 0.001f;

                particle->AddForce(g_InterBoidStiffness * boidComponent->m_interBoidStiffnessMult * (toBoidL - boidComponent->m_distanceMult * g_InterBoidDistance) / toBoidL * toBoid);
            }
        }
    }

    DeclareComponentSystem(BLADemos, UpdateBoidTracers, InputComponents(TransformComponent), OutputComponents(BoidComponent));
    RegisterComponentSystem(UpdateBoidTracers, Dependencies(IntegrateBoidSystem));

    template <>
    void UpdateBoidTracers::Execute(SystemObjectsIterator& systemObjects, InputComponents<TransformComponent> iTransforms, OutputComponents<BoidComponent> oBoids)
    {
        for(auto systemObject : systemObjects) 
        {
            BoidComponent* boid = oBoids[systemObject];

            boid->m_pastPositions.Push(iTransforms[systemObject]->GetLocalTransform().GetPosition());

            int size = boid->m_pastPositions.GetCount();
            for (int i = 0; i < size - 1; i++)
            {
                //DebugDraw::DrawLine(boid->m_pastPositions.Peek(i), boid->m_pastPositions.Peek(i + 1), boid->m_color);
            }
        }
    }

    DefineConsoleCommand(void, CreateBoidSystem, int numberOfBoids)
    {
        Scene* scene = EngineInstance::GetSingletonInstance()->GetWorkingScene();

        scene->CreateObject(BlaStringId("BoidTarget"));

        std::uniform_real_distribution<float> dist(0.2f, 1.f);
        std::normal_distribution<float> normalDist(0.7f, 0.3f);

        MeshAsset m("BoidSphere");
        m.m_triangleMesh = PrimitiveGeometry::MakeCube();

        AssetManager::GetSingletonInstance()->SaveTriangleMesh(&m);

        for (int i = 0; i < numberOfBoids; i++)
        {
            GameObject ref = scene->CreateObject(GenerateBlaStringId("Boid" + std::to_string(i))); // TODO: Please don't ...

            BoidComponent* boidComponent = ref.CreateComponent<BoidComponent>();
            ref.CreateComponent<ParticleComponent>();
            MeshRendererComponent* r = ref.CreateComponent<MeshRendererComponent>();

            r->MeshAssetName = "BoidSphere";
            r->MaterialName = "BlankDiffuseMat";

            boidComponent->m_color = blaVec3(dist(g_dgen), dist(g_dgen), dist(g_dgen));
            boidComponent->m_hominStiffnessMult = dist(g_dgen);
            boidComponent->m_interBoidStiffnessMult = dist(g_dgen);
            boidComponent->m_distanceMult = normalDist(g_dgen);
            boidComponent->m_target = GameObject(BlaStringId("BoidTarget"));

            blaScaledTransform s(blaVec3(0.2f), blaPosQuat(blaVec3(dist(g_dgen), dist(g_dgen), dist(g_dgen)), blaPosQuat::QuatIdentity()));

            ref.GetComponent<TransformComponent>()->SetTransform(s);
        }
    }
}
