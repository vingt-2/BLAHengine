#include "DirectionalLight.h"

	DirectionalLight::DirectionalLight(vec3 direction)
	{
		m_lightDirection = direction;
	}

	DirectionalLight::~DirectionalLight()
	{

	}

	void DirectionalLight::Update()
	{
		this->UpdateTransform();
	}

	void DirectionalLight::SetDirection(vec3 direction)
	{
		m_lightDirection = direction;
	}

	vec3 DirectionalLight::GetDirection()
	{
		return transform->LocalDirectionToWorld(m_lightDirection);
	}