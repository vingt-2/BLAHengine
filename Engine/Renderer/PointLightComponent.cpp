#include "PointLightComponent.h"

#include <Core/GameObject.h>

#include "Core/Scene.h"
#include "Core/RenderingManager.h"
#include "Core/TransformComponent.h"

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(PointLightComponent)
EXPOSE(m_radius)
END_DESCRIPTION()

// Norm of the default m_scale for Object Transforms...
#define BLA_SCALE_NORM 1.73205080757f

void PointLightComponent::Init()
{
    m_radius = 1.f;
    m_renderTicket = Scene::GetSingletonInstance()->GetRenderingManager()->RegisterPointLight(this);
}

void PointLightComponent::Shutdown()
{
    Scene::GetSingletonInstance()->GetRenderingManager()->CancelPointLightTicket(this);
}

void PointLightComponent::Update()
{
    m_position = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetPosition();
}
