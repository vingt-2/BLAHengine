#pragma once
#include "../../Std/std.h"
#include "../../Tools/OBJImport.h"
#include "../Game/GameObjects/Camera.h"
#include "LightRender.h"

class RenderObject
{
public:
	virtual ~RenderObject(){};
private:

};

class Renderer
{
public:
	enum RendererType
	{
		WIN_GL_33,
		OSX_GL_32
	};

	PerspectiveCamera m_mainRenderCamera;

	vector<RenderObject*> m_renderPool;
	vector<RenderObject*> m_gizmoRenderPool;
	RenderObject* debug_sphere;
	vector<vec3> debug_sphere_requests;

	// Rendering Related
	virtual bool Update() = 0;
	virtual RenderObject* LoadRenderObject(const MeshRenderer& object,int type) = 0;
	virtual bool		  CancelRender(const MeshRenderer& object) = 0;

	// Windowing Related
	virtual vec2 GetCursorPosition() = 0;
	
	bool GetStatus()        const   {	return m_isContextEnabled;	}
	vec2 GetRenderSize()    const   {   return m_renderSize;  }
	
	void SetCamera(Camera* camera);

	Renderer(char* windowTitle,bool isFullScreen);
	Renderer(char* windowTitle, bool isFullScreen, vec2 screenSize);
	~Renderer(void);

protected:

	GLFWwindow* m_glfwWindow;
	
	ivec2 m_renderSize;
	bool m_isContextEnabled;
	bool m_isFullScreen;
	
	virtual GLFWwindow* InitializeContext(char* windowTitle) = 0;
};

