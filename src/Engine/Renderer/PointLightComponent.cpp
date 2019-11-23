#include "PointLightComponent.h"

#include <Engine/Core/GameObject.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(PointLightComponent)
END_DESCRIPTION()

// Norm of the default m_scale for Object Transforms...
#define BLA_SCALE_NORM 1.73205080757f

PointLightComponent::PointLightComponent(GameObjectReference parentObject) :
    GameComponent(parentObject),
    m_radiosity(blaVec3(1.f, 1.f, 1.f))
{}

void PointLightComponent::Update()
{
    ObjectTransform& t = GetObjectTransform();
    m_position = t.GetPosition();
    m_radius = glm::length(t.m_scale);
}
