#include "DirectionalLightComponent.h"
#include <Engine/Core/CameraComponent.h>
#include <Engine/Core/GameObject.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(DirectionalLightComponent)
END_DESCRIPTION()

DirectionalLightComponent::DirectionalLightComponent(GameObjectReference parentObject):
    GameComponent(parentObject)
{
    m_lightDirection = blaVec3(0.f);
    m_renderTicket = 0;
}

DirectionalLightComponent::~DirectionalLightComponent()
{}

void DirectionalLightComponent::Update()
{
    blaVector<CameraComponent*> cameras = GetParentObject()->GetComponents<CameraComponent>();
    if (!cameras.size())
    {
        CameraComponent* shadowCamera = BLA_CREATE_COMPONENT(CameraComponent, GetParentObject());
        shadowCamera->m_isShadowMapCamera = true;

        shadowCamera->Update();
    }
}

void DirectionalLightComponent::SetDirection(blaVec3 direction)
{
    m_lightDirection = direction;
}

blaVec3 DirectionalLightComponent::GetDirection()
{
    const ObjectTransform& t = GetParentObject()->GetTransform();
    return t.LocalDirectionToWorld(m_lightDirection);
}