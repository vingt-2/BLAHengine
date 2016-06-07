#include "ShadowRender.h"

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
}

void PerspectiveShadowRender::Update()
{
	m_shadowCamera.Update();
}
