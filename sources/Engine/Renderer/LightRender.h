#pragma once
#include "ShadowRender.h"

namespace BLAengine
{
	class BLACORE_API DirectionalLightRender
	{
	public:
		GLuint m_lightRenderPrgmID;

		DirectionalShadowRender m_shadowRender;
	};

	class BLACORE_API PointLightRender
	{
	public:

		GLuint m_pointLightPrgmID;

		Transform* m_transform;

		vector<vec3> m_sphereMesh;
	};
}