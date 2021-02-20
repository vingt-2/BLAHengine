// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "MeshRendererComponent.h"
#include "Rendering/RenderPass.h"
#include "Core/GameObject.h"
#include "Core/TransformComponent.h"
#include "Assets/AssetsManager.h"
#include "Core/Scene.h"

using namespace BLA;

// Todo: move camera component to rendering ...
#include "Core/CameraComponent.h"

RegisterRenderPass(MeshRendererComponent::MeshGeometryPass);

RegisterRenderPass(MeshRendererComponent::TestMeshPass)

BeginBehaviorDescription(MeshRendererComponent, Dependencies(RootSystem))
Expose(m_meshAssetName)
Expose(m_Render)
EndBehaviorDescription()

blaStringId g_testMeshPassId;

void MeshRendererComponent::Init()
{
    
}

bool MeshRendererComponent::AssignTriangleMesh(MeshAsset* mesh)
{
    m_mesh = mesh;
    return true;
}

const blaMat4* MeshRendererComponent::GetTransformMatrix() const
{
    return m_modelTransformMatrix.GetData();
}

void MeshRendererComponent::Shutdown()
{
    //Scene::GetSingletonInstance()->GetRenderingManager()->CancelMeshRendererTicket(this);
}

void OnOffscreenRenderTarget(RenderTarget* rt)
{
    OffscreenRenderTarget* offscreenRenderTarget = static_cast<OffscreenRenderTarget*>(rt);

    Renderer* renderer = Renderer::GetSingletonInstance();
    if (g_testMeshPassId)
    {
        if(MeshRendererComponent::TestMeshPass::RenderPassInstance* instance = renderer->GetRenderPassInstance<MeshRendererComponent::TestMeshPass>(g_testMeshPassId))
        {
            MeshRendererComponent::TestMeshPass::RenderPassAttachment::Color colorAttachments(Gpu::ColorAttachmentDesc(renderer->m_offscreenBuffer.m_color.m_p, 0.f, 0.f, 0.f, 1.f));
            // Make Attachment from renderer offscreen images:
            MeshRendererComponent::TestMeshPass::RenderPassAttachment attachment(colorAttachments, Gpu::DepthStencilAttachmentDesc(renderer->m_offscreenBuffer.m_depth.m_p, 1.f, 0.f));

            instance->ResetAttachment(attachment);
        }
    }
}

void MeshRendererComponent::Update()
{
    Renderer* renderer = Renderer::GetSingletonInstance();

    if(!g_testMeshPassId)
    {
        Gpu::ShaderProgram vertexShader(Gpu::ShaderProgram::Type::VertexShader, "./resources/shaders/Vulkan/Engine/TestMeshPassVert.spv");
        Gpu::ShaderProgram fragmentShader(Gpu::ShaderProgram::Type::FragmentShader, "./resources/shaders/Vulkan/Engine/TestMeshPassFrag.spv");

        vertexShader.Submit();
        fragmentShader.Submit();

        Gpu::RenderPassProgram program;
        program.m_shaders.push_back(vertexShader);
        program.m_shaders.push_back(fragmentShader);

        TestMeshPass::RenderPassAttachment::Color colorAttachments(Gpu::ColorAttachmentDesc(renderer->m_offscreenBuffer.m_color.m_p, 0.f, 0.f, 0.f, 1.f));
        // Make Attachment from renderer offscreen images:
        TestMeshPass::RenderPassAttachment attachment(colorAttachments, Gpu::DepthStencilAttachmentDesc(renderer->m_offscreenBuffer.m_depth.m_p, 1.f, 0.f));

        g_testMeshPassId = BlaStringId("TestMeshPass");

        renderer->AddRenderPassInstance<TestMeshPass>(g_testMeshPassId, attachment, program);

        renderer->m_offscreenBuffer.RegisterOnChangeCallback(OnOffscreenRenderTarget);
    }
    
    bool validState = true;
    if(m_mesh == nullptr || m_mesh->GetName() != m_meshAssetName)
    {
        Asset* meshAsset = nullptr;
        if(AssetManager::GetSingletonInstance()->GetAsset(m_meshAssetName, meshAsset) == AssetManager::TriangleMeshAsset)
        {
            AssignTriangleMesh(static_cast<MeshAsset*>(meshAsset));
        }
        else
        {
            validState = false;
        }
    }

    CameraComponent* camera = Core::Scene::GetSingletonInstance()->GetMainCamera();

    if (!camera) return;
    
    blaMat4 localToWorld;
    if (!GetOwnerObject().IsValid())
    {
        localToWorld = blaMat4(0);
    }
    else
    {
        GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetScaledTransformMatrix(localToWorld);
    }

    *m_modelTransformMatrix.GetData() = localToWorld;
    *m_MVP.GetData() = camera->m_camera.m_worldToClipSpace * localToWorld;

    //if(!m_material || m_material->GetName() != MaterialName)
    //{
    //    Asset* materialAsset = nullptr;
    //    if (AssetManager::GetSingletonInstance()->GetAsset(MaterialName, materialAsset) == AssetManager::AssetType::MaterialAsset)
    //    {
    //        AssignMaterial(static_cast<Material*>(materialAsset), 0);
    //    }
    //    else
    //    {
    //        validState = false;
    //    }
    //}

    bool render = m_Render && validState;
    
    if(m_renderTicket != 0 && !render)
    {
        m_renderTicket = 0;
    }

    if(m_renderTicket == 0 && render)
    {
        m_renderTicket = 1;

        {
            CameraComponent* camera = Core::Scene::GetSingletonInstance()->GetMainCamera();
            m_camera.AttachCamera(camera);
            RenderData& rd = m_mesh->m_triangleMesh.m_renderData;

            m_vertPos = new Gpu::StaticBuffer<blaVec3>(static_cast<blaU32>(rd.m_vertPos.size()), Gpu::BaseStaticBuffer::Usage::VertexBuffer);
            m_vertPos->CopyFromVector(rd.m_vertPos);

            m_vertNormal = new Gpu::StaticBuffer<blaVec3>(static_cast<blaU32>(rd.m_vertNormal.size()), Gpu::BaseStaticBuffer::Usage::VertexBuffer);
            m_vertNormal->CopyFromVector(rd.m_vertNormal);

            m_vertUV = new Gpu::StaticBuffer<blaVec2>(static_cast<blaU32>(rd.m_vertUVs.size()), Gpu::BaseStaticBuffer::Usage::VertexBuffer);
            m_vertUV->CopyFromVector(rd.m_vertUVs);

            m_vertTangent = new Gpu::StaticBuffer<blaVec3>(static_cast<blaU32>(rd.m_vertTangent.size()), Gpu::BaseStaticBuffer::Usage::VertexBuffer);
            m_vertTangent->CopyFromVector(rd.m_vertTangent);

            m_vertBiTangent = new Gpu::StaticBuffer<blaVec3>(static_cast<blaU32>(rd.m_vertBiTangent.size()), Gpu::BaseStaticBuffer::Usage::VertexBuffer);
            m_vertBiTangent->CopyFromVector(rd.m_vertBiTangent);

            m_indices = new Gpu::StaticBuffer<blaU32>(static_cast<blaU32>(rd.m_triangleIndices.size()), Gpu::BaseStaticBuffer::Usage::IndexBuffer);
            m_indices->CopyFromVector(rd.m_triangleIndices);

            m_vertPos->Submit();
            m_vertUV->Submit();
            m_vertNormal->Submit();
            m_vertTangent->Submit();
            m_vertBiTangent->Submit();
            m_indices->Submit();

            const TestMeshPass::RenderPassObject::InstanceVertexAttributes meshVAs(*m_vertPos);
            const TestMeshPass::RenderPassObject::InstanceUniformValues meshUniforms(m_modelTransformMatrix, m_MVP);
            const TestMeshPass::RenderPassObject::InstanceOpaqueValues opaques;

            m_renderPassObjects = new TestMeshPass::RenderPassObject(*m_indices, meshVAs, meshUniforms, opaques);

            if(TestMeshPass::RenderPassInstance* testMeshPassInstance = renderer->GetRenderPassInstance<TestMeshPass>(g_testMeshPassId))
            {
                testMeshPassInstance->RegisterRenderPassObject(*m_renderPassObjects);
            }
        }
    }
}
