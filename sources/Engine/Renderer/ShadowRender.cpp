#include "ShadowRender.h"
using namespace BLAengine;

mat4 DirectionalShadowRender::getShadowViewProjection()
{
	return m_shadowCamera.m_ViewProjection;
}

mat4 PerspectiveShadowRender::getShadowViewProjection()
{
	return m_shadowCamera.m_ViewProjection;
}

void DirectionalShadowRender::Update()
{
	m_shadowCamera.Update();
	m_shadowDirection = m_shadowCamera.m_attachedCamera->GetObjectTransform().LocalDirectionToWorld(vec3(0, 0, 1.f));
}

void PerspectiveShadowRender::Update()
{
	m_shadowCamera.Update();

	m_shadowDirection = m_shadowCamera.m_attachedCamera->GetObjectTransform().LocalDirectionToWorld(vec3(0, 0, 1.f));
}
