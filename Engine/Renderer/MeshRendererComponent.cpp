// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "MeshRendererComponent.h"

#include "Core/GameObject.h"
#include "Core/TransformComponent.h"
#include "Assets/AssetsManager.h"

using namespace BLA;

#include "Core/CameraComponent.h"
#include "Renderer/Vulkan/VulkanRenderer.h"

DeclareRenderPass(
    MeshTestRenderPass,
    VertexAttributes(
        blaVec3, // Model Pos
        blaVec3), // normal 
    UniformValues(blaMat4),
    1)

RegisterRenderPass(MeshTestRenderPass)

// TODO: Find a better way to register the rendering components ...
#include "Core/Scene.h"

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
    return &m_modelTransformMatrix;
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
        int renderTicket = 1;
        VulkanRenderer* renderer = nullptr;
        if(renderer)
        {
            CameraComponent* camera = Core::Scene::GetSingletonInstance()->GetMainCamera();
            m_camera.AttachCamera(camera);
            RenderData& rd = m_mesh->m_triangleMesh.m_renderData;

            m_vertPos = *Gpu::StaticBuffer<blaVec3>::New(rd.m_vertPos.size());
            memcpy_s(m_vertPos->GetData(), sizeof(blaVec3) * m_vertPos->GetLength(), rd.m_vertPos.data(), rd.m_vertPos.size() * sizeof(blaVec3));

            m_vertNormal = *Gpu::StaticBuffer<blaVec3>::New(rd.m_vertNormal.size());
            memcpy_s(m_vertNormal->GetData(), sizeof(blaVec3) * m_vertNormal->GetLength(), rd.m_vertNormal.data(), rd.m_vertNormal.size() * sizeof(blaVec3));

            m_indices = *Gpu::StaticBuffer<blaU32>::New(rd.m_triangleIndices.size());
            memcpy_s(m_indices->GetData(), sizeof(blaVec3) * m_indices->GetLength(), rd.m_triangleIndices.data(), rd.m_triangleIndices.size() * sizeof(blaVec3));

            m_vertPos.Submit();
            m_vertNormal.Submit();
            m_indices.Submit();

            const MeshTestRenderPass::RenderPassInstance::InstanceVertexAttributes meshVAs(*m_vertPos.Get(), *m_vertNormal.Get());
            const MeshTestRenderPass::RenderPassInstance::InstanceUniformValues meshUniforms(*GetTransformMatrix());

            MeshTestRenderPass::RenderPassInstance renderPassInstance(*m_indices.Get(), meshVAs, meshUniforms);

            // renderer->GetRenderPassManager()->AddRenderPassInstance<MeshTestRenderPass>(renderPassInstance);
        }
    }

    if (!GetOwnerObject().IsValid())
        m_modelTransformMatrix = blaMat4(0);
    else
        GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetScaledTransformMatrix(m_modelTransformMatrix);
}
