#include "CameraComponent.h"
#include <Engine/Core/GameObject.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(CameraComponent)
END_DESCRIPTION()

CameraComponent::CameraComponent(GameObjectReference parentObject) :
    GameComponent(parentObject),
    m_isShadowMapCamera(false)
{
    UpdateView();
}

void CameraComponent::UpdateView()
{
    if (!GetOwnerObject().IsValid())
        return;

    m_viewTransform.SetPosition(GetOwnerObject()->GetTransform().GetPosition());
    m_viewTransform.SetRotation(GetOwnerObject()->GetTransform().GetRotation());

    m_viewTransform.m_posQuat = m_viewTransform.m_posQuat.GetInverse();
}

void CameraComponent::Update()
{
    // Regular updates
    UpdateView(); // View ObjectTransform update
}
