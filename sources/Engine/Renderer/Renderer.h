#pragma once
#include "../../Std/std.h"
#include "./../Game/Debug.h"
#include "../../Tools/OBJImport.h"
#include "../Game/GameObjects/Camera.h"

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
	
	Camera* m_mainCamera;
	Debug* m_debug;
	

	vector<RenderObject*> m_renderPool;

	//////
	//
	//HardCode
	//////
	vec3 m_directionalLight;

	virtual bool Update() = 0;
	virtual RenderObject* LoadRenderObject(const MeshRenderer& object) = 0;

	void Resize(int xRes,int yRes);
	
	bool GetStatus()        const   {	return isContextEnabled;	}
	GLFWwindow* GetWindow() const   {	return glfwWindow;	}
	vec2 GetRenderSize()    const   {   return renderSize;  }
	

	Renderer(char* windowTitle,bool isFullScreen);
	Renderer(char* windowTitle, bool isFullScreen, vec2 screenSize);
	~Renderer(void);

protected:

	GLFWwindow* glfwWindow;
	
	
	vec2 renderSize;
	bool isContextEnabled;
	bool isFullScreen;
	
	virtual GLFWwindow* InitializeContext(char* windowTitle) = 0;
};

