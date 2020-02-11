#include "CameraComponent.h"
#include <Core/GameObject.h>
#include "Core/TransformComponent.h"
using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(CameraComponent)
EXPOSE(m_isShadowMapCamera)
END_DESCRIPTION()

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
