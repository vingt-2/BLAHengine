// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "MeshRendererComponent.h"

#include "Core/GameObject.h"
#include "Core/TransformComponent.h"
#include "RenderBackend.h"
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
        VulkanRenderer* renderer = VulkanRenderer::GetSingletonInstance();
        if(renderer)
        {
            CameraComponent* camera = Scene::GetSingletonInstance()->GetMainCamera();
            m_camera.AttachCamera(camera);
            RenderData& rd = m_mesh->m_triangleMesh.m_renderData;
            const MeshTestRenderPass::RenderPassInstance::InstanceVertexAttributes meshVAs(rd.m_vertPos, rd.m_vertNormal);
            const MeshTestRenderPass::RenderPassInstance::InstanceUniformValues meshUniforms(*GetTransformMatrix());

            MeshTestRenderPass::RenderPassInstance renderPassInstance(meshVAs, meshUniforms);
            renderPassInstance.m_indices = rd.m_triangleIndices;

            // renderer->GetRenderPassManager()->AddRenderPassInstance<MeshTestRenderPass>(renderPassInstance);
        }
    }

    if (!GetOwnerObject().IsValid())
        m_modelTransformMatrix = blaMat4(0);
    else
        GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetScaledTransformMatrix(m_modelTransformMatrix);
}
