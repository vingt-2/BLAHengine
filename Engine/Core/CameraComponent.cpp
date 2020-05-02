#include "CameraComponent.h"
#include "Core/GameObject.h"
#include "Core/TransformComponent.h"
using namespace BLA;

BeginComponentDescription(CameraComponent)
Expose(m_isShadowMapCamera)
EndComponentDescription()

void CameraComponent::Init()
{
    UpdateView();
}

void CameraComponent::UpdateView()
{
    if (!GetOwnerObject().IsValid())
        return;

    blaScaledTransform t = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();

    m_viewTransform.m_posQuat = t.m_posQuat.GetInverse();
}

void CameraComponent::Update()
{
    // Regular updates
    UpdateView(); // View TransformComponent update
}
