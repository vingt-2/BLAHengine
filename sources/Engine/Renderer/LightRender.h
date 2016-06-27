#pragma once
#include "ShadowRender.h"

class DirectionalLightRender
{
public:
	GLuint m_lightRenderPrgmID;

	DirectionalShadowRender m_shadowRender;
};

class PointLightRender
{
public:

	GLuint m_pointLightPrgmID;

	Transform* m_transform;

	vector<vec3> m_sphereMesh;
};