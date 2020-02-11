#include "DirectionalLightComponent.h"
#include "Core/CameraComponent.h"
#include "Core/TransformComponent.h"
#include <Core/GameObject.h>

#include "Core/Scene.h"
#include "Core/RenderingManager.h"

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(DirectionalLightComponent)
END_DESCRIPTION()

void DirectionalLightComponent::Update()
{
    CameraComponent* shadowCamera = GetOwnerObject().GetComponent<CameraComponent>();
    shadowCamera->Update();
}

void DirectionalLightComponent::SetDirection(blaVec3 direction)
{
    m_lightDirection = direction;
}

blaVec3 DirectionalLightComponent::GetDirection()
{
    return GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().LocalDirectionToWorld(m_lightDirection);
}

void DirectionalLightComponent::Init()
{
    CameraComponent* shadowCamera = GetOwnerObject().GetComponent<CameraComponent>();
    if (!shadowCamera)
    {
        shadowCamera = GetOwnerObject().CreateComponent<CameraComponent>();
        shadowCamera->m_isShadowMapCamera = true;
    }

    m_renderTicket = Scene::GetSingletonInstance()->GetRenderingManager()->RegisterDirectionalLight(this, shadowCamera);
}

void DirectionalLightComponent::Shutdown()
{
    Scene::GetSingletonInstance()->GetRenderingManager()->CancelDirectionalLightTicket(this);
}
