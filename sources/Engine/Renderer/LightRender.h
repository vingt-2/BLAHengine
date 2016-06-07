#pragma once
#include "ShadowRender.h"

class DirectionalLightRender
{
public:
	GLuint m_lightRenderPrgmID;

	DirectionalShadowRender m_shadowRender;
};