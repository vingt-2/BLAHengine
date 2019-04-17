#include "CameraComponent.h"
#include <Engine/Game/GameObject.h>

using namespace BLAengine;

CameraComponent::CameraComponent(GameObjectReference parentObject) :
    GameComponent(parentObject),
    m_fovSize(glm::vec2(1,1)),
    m_isShadowMapCamera(false)
{
    UpdateView();
    m_isControlEnabled = false;
}

CameraComponent::~CameraComponent(void)
{

}


void CameraComponent::UpdateView()
{
    if (!GetParentObject().IsValid())
        return;

    m_viewTransform.SetPosition(GetParentObject()->GetTransform().GetPosition());
    m_viewTransform.SetRotation(GetParentObject()->GetTransform().GetRotation());

    m_viewTransform.m_posQuat = m_viewTransform.m_posQuat.GetInverse();
}

void CameraComponent::Update()
{
    // Regular updates
    UpdateView(); // View ObjectTransform update
}
