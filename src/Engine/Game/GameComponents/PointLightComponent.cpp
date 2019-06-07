#include "PointLightComponent.h"

#include <Engine/Game/GameObject.h>

using namespace BLAengine;

// Norm of the default m_scale for Object Transforms...
#define BLA_SCALE_NORM 1.73205080757f

PointLightComponent::PointLightComponent(GameObjectReference parentObject):
    GameComponent(parentObject),
    m_radiosity(blaVec3(1.f,1.f,1.f))
{}

blaVec3 PointLightComponent::GetLightPosition() const
{
    return GetObjectTransform().GetPosition();
}

float PointLightComponent::GetLightRadius() const
{
    return glm::length(GetObjectTransform().m_scale) / BLA_SCALE_NORM;
}

void PointLightComponent::Update()
{

}
