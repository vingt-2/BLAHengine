#include "MeshRendererComponent.h"

#include <Engine/Core/GameObject.h>
#include <Common/RenderBackend.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(MeshRendererComponent)
EXPOSE(m_renderType)
END_DESCRIPTION()

MeshRendererComponent::MeshRendererComponent(GameObjectReference parentObject) :
    GameComponent(parentObject),
    m_renderType(GL_TRIANGLES),
    m_renderTicket(0)
{
    this->m_mesh = nullptr;
}

bool MeshRendererComponent::AssignTriangleMesh(MeshAsset* mesh)
{
    m_mesh = mesh;

    return true;
}

bool MeshRendererComponent::AssignMaterial(Material* material, int matIndx)
{
    m_materials.push_back(material);

    return true;
}

const blaMat4* MeshRendererComponent::GetTransformMatrix() const
{
    return &m_modelTransformMatrix;
}

void MeshRendererComponent::Update()
{
    if (!GetOwnerObject().IsValid())
        m_modelTransformMatrix = blaMat4(0);
    else
        GetOwnerObject()->GetTransform().GetScaledTransformMatrix(m_modelTransformMatrix);
}
