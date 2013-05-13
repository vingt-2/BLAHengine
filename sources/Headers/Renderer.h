#pragma once
#include "OBJImport.h"
#include "Camera.h"
#include "std.h"
#include "Debug.h"

class Renderer
{
public:
	Camera* mainCamera;
	Debug* debug;

	vector<MeshRenderer*> renderVector;


	bool Update();
	void Resize(int xRes,int yRes);
	bool GetStatus(){	return isContextEnabled;	}
	GLFWwindow* GetWindow() {	return glfwWindow;	}



	Renderer(char* windowTitle);
	Renderer(char* windowTitle, vec2 screenSize);
	~Renderer(void);

private:

	vec2 screenSize;
	GLFWwindow* glfwWindow;
	GLFWwindow* InitializeContext(char* windowTitle,vec2 screenSize);
	bool isContextEnabled;

};

