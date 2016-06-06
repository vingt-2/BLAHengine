#include "RenderCamera.h"

class ShadowRender
{
public:

	bool m_isSetup;

	//OpenGL
	GLuint m_depthTexture;
	GLuint m_shadowPrgmID;
	GLuint m_shadowBuffer;

	int m_bufferSize;

	virtual mat4 getShadowViewProjection() = 0;
	virtual void Update() = 0;  // <-- m_shadowCamera.Update();
};

class DirectionalShadowRender : ShadowRender
{
public:
	mat4 getShadowViewProjection()
	{
		return m_shadowCamera.m_ViewProjection;
	}

	OrthographicCamera m_shadowCamera;
};

class PerspectiveShadowRender : ShadowRender
{
public:
	mat4 getShadowViewProjection()
	{
		return m_shadowCamera.m_ViewProjection;
	}

	OrthographicCamera m_shadowCamera;

};

