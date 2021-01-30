// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "MeshRendererComponent.h"
#include "Rendering/RenderPass.h"
#include "Core/GameObject.h"
#include "Core/TransformComponent.h"
#include "Assets/AssetsManager.h"
#include "Gpu/Interface.h"
#include "Core/Scene.h"

using namespace BLA;

// Todo: move camera component to rendering ...
#include "Core/CameraComponent.h"

class RenderPassAttachmentMesh {};

DeclareRenderPass(
    MeshGeometryPass,
    RenderPassAttachmentMesh,
    VertexAttributes(
        blaVec3, // ModelPos
        blaVec2, // uv
        blaVec3, // normal
        blaVec3,  // tangent
        blaVec3), // bi-tangent
    UniformValues(
        blaMat4, // mvp
        blaMat4)) // model

RegisterRenderPass(MeshGeometryPass);

DeclareRenderPass(
    TestMeshPass,
    RenderPassAttachmentMesh,
    VertexAttributes(
        blaVec3), // ModelPos
    UniformValues(
        blaMat4, // mvp
        blaMat4)) // model

RegisterRenderPass(TestMeshPass)

BeginBehaviorDescription(MeshRendererComponent, Dependencies(RootSystem))
Expose(m_meshAssetName)
Expose(m_Render)
EndBehaviorDescription()

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

void MeshRendererComponent::Update()
{
    
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
            memcpy_s(m_vertPos->GetData(), sizeof(blaVec3) * m_vertPos->GetLength(), rd.m_vertPos.data(), rd.m_vertPos.size() * sizeof(blaVec3));
            
            m_indices = new Gpu::StaticBuffer<blaU32>(static_cast<blaU32>(rd.m_triangleIndices.size()), Gpu::BaseStaticBuffer::Usage::IndexBuffer);
            memcpy_s(m_indices->GetData(), sizeof(blaU32) * m_indices->GetLength(), rd.m_triangleIndices.data(), rd.m_triangleIndices.size() * sizeof(blaU32));

            m_vertPos->Submit();
            m_indices->Submit();

            const TestMeshPass::RenderPassObject::InstanceVertexAttributes meshVAs(*m_vertPos);
            const TestMeshPass::RenderPassObject::InstanceUniformValues meshUniforms(m_modelTransformMatrix, m_MVP);

            // Leak ... of course !
            TestMeshPass::RenderPassObject* renderPassObject = new TestMeshPass::RenderPassObject(*m_indices, meshVAs, meshUniforms);
            
            Gpu::Interface::GetSingletonInstance()->RegisterRenderPassObject<TestMeshPass>(*renderPassObject);
        }
    }
}
