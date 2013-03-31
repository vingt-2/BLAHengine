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

	vec2 screenSize;

	void Resize(int xRes,int yRes);
	bool InitializeContext(char* windowTitle);
	bool Update();

	Renderer(void);
	~Renderer(void);
};

