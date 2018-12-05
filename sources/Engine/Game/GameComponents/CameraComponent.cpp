#include "CameraComponent.h"
#include "../GameObject.h"
using namespace BLAengine;

CameraComponent::CameraComponent():
    m_fovSize(glm::vec2(1,1)),
    GameComponent(),
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
    if (!m_parentObject)
        return;

    m_viewTransform.SetPosition(m_parentObject->GetTransform().GetPosition());
    m_viewTransform.SetRotation(m_parentObject->GetTransform().GetRotation());

    m_viewTransform.m_transform = m_viewTransform.m_transform.GetInverse();
}

void CameraComponent::Update()
{
    // Regular updates
    UpdateView(); // View ObjectTransform update
}
