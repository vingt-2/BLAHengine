#include "MeshRendererComponent.h"

#include <Engine/Core/GameObject.h>
#include <Common/RenderBackend.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(MeshRendererComponent)
END_DESCRIPTION()

MeshRendererComponent::MeshRendererComponent(GameObjectReference parentObject) : 
    GameComponent(parentObject),
    m_renderType    (GL_TRIANGLES),
    m_renderTicket  (0),
    m_modelTransformMatrix(new blaMat4(0))
{
    this->m_mesh = nullptr;
} 

MeshRendererComponent::~MeshRendererComponent(void)
{
    free(m_modelTransformMatrix);
}

blaString MeshRendererComponent::ToString()
{
    return "MeshRendererComponent ToString PLACEHOLDER";
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

blaMat4* MeshRendererComponent::GetTransformMatrix() const
{
    blaMat4* a = m_modelTransformMatrix;
    return a;
}

void MeshRendererComponent::Update()
{
    if (!GetParentObject().IsValid())
        *m_modelTransformMatrix = blaMat4(0);
    else
        GetParentObject()->GetTransform().GetScaledTransformMatrix(*m_modelTransformMatrix);
}
