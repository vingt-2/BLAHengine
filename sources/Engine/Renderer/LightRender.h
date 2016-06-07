#pragma once
#include "ShadowRender.h"

class DirectionalLightRender
{
public:
	vec3 m_lightDirection;
	GLuint m_lightRenderPrgmID;

	DirectionalShadowRender m_shadowRender;
};