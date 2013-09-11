#include "PointLight.h"

PointLight::PointLight():
    m_lightPosition(vec3(0)),m_lightIntensity(1.f),m_lightRadius(1.f)
{}

PointLight::PointLight(vec3 position,float radius, float intensity)
{
    m_lightPosition = position;
    m_lightRadius = radius;
    m_lightIntensity = intensity;
}

void PointLight::Update()
{
    this->UpdateTransform();
}