#include "RenderCamera.h"

class ShadowRender
{
public:
	virtual mat4 getShadowViewProjection() = 0;
	virtual void Update() = 0;  // <-- m_shadowCamera.Update();
};

class DirectionalShadowRender : ShadowRender
{
public:
	bool m_isSetup;
	vec3 m_shadowDirection;

	//OpenGL
	GLuint m_depthTexture;
	GLuint m_shadowPrgmID;
	GLuint m_shadowBuffer;

	int m_bufferSize;

	mat4 getShadowViewProjection();
	void Update();

	OrthographicCamera m_shadowCamera;
};

class PerspectiveShadowRender : ShadowRender
{
public:
	bool m_isSetup;
	vec3 m_shadowDirection;

	//OpenGL
	GLuint m_depthTexture;
	GLuint m_shadowPrgmID;
	GLuint m_shadowBuffer;

	int m_bufferSize;

	mat4 getShadowViewProjection();
	void Update();

	OrthographicCamera m_shadowCamera;

};

