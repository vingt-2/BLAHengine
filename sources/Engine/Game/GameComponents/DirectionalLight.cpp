#include "DirectionalLight.h"
#include "../GameObject.h"
using namespace BLAengine;

DirectionalLight::DirectionalLight(vec3 direction)
{
	m_lightDirection = direction;
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Update()
{
}

void DirectionalLight::SetDirection(vec3 direction)
{
	m_lightDirection = direction;
}

vec3 DirectionalLight::GetDirection()
{
	const Transform& t = m_parentObject->GetTransform();
	return t.LocalDirectionToWorld(m_lightDirection);
}