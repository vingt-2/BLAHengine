#include "MeshRendererComponent.h"

#include "Core/GameObject.h"
#include "Core/TransformComponent.h"
#include "RenderBackend.h"
#include "Assets/AssetsManager.h"

// TODO: Find a better way to register the rendering components ...
#include "Core/Scene.h"

using namespace BLA;

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
    Scene::GetSingletonInstance()->GetRenderingManager()->CancelMeshRendererTicket(this);
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
        Scene::GetSingletonInstance()->GetRenderingManager()->CancelMeshRendererTicket(this);
        m_renderTicket = 0;
    }

    if(m_renderTicket == 0 && render)
    {
        m_renderTicket = Scene::GetSingletonInstance()->GetRenderingManager()->RegisterMeshRenderer(this);
    }

    if (!GetOwnerObject().IsValid())
        m_modelTransformMatrix = blaMat4(0);
    else
        GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetScaledTransformMatrix(m_modelTransformMatrix);
}
