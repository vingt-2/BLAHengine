#pragma once
#include "../../Common/System.h"
#include "../../Common/StdInclude.h"

class BLACORE_API RenderWindow
{
public:
	virtual void CreateWindow(int sizeX, int sizeY, bool isFullScreen) = 0;
	virtual void CreateOpenGLContext() = 0;
	virtual void GetMaxGLVersion() = 0;

	virtual void MakeGLContextCurrent() = 0;
	virtual void UpdateWindowAndBuffers() = 0;

	virtual void GetSize(int &width, int &height) = 0;

	virtual bool isFullScreen() = 0;

	virtual void SetWindowTitle(std::string title) = 0;
	virtual std::string GetWindowTitle() = 0;

	virtual void GetMouse(double &x, double &y) = 0;

	virtual void SetMouseXY() = 0;
	
	virtual bool GetMousePressed(int button) = 0;
	virtual bool GetKeyPressed(int key) = 0;
};

class GLContextInfo 
{
	int major, minor;

};

class BLACORE_API GLFWRenderWindow : RenderWindow
{
public:

	virtual void CreateWindow(int sizeX, int sizeY, bool isFullScreen);
	virtual void CreateOpenGLContext();
	virtual void GetMaxGLVersion();
	virtual void MakeGLContextCurrent();
	virtual void UpdateWindowAndBuffers();
	virtual void GetSize(int &width, int &height);

	virtual bool isFullScreen();

	virtual void SetWindowTitle(std::string title);
	virtual std::string GetWindowTitle();

	virtual void GetMouse(double &x, double &y);

	virtual void SetMouseXY();

	virtual bool GetKeyPressed(int key);
	virtual bool GetMousePressed(int button);

private:

	GLFWwindow* m_glfwWindow;

	int m_width, m_height;

	bool m_isFullscreen;

};
