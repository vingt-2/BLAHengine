#pragma once

#include "OBJImport.h"
#include "Camera.h"
#include "std.h"
class Renderer
{
public:
	Camera* mainCamera;

	vector<MeshRenderer*> renderVector;

	void Resize(int xRes,int yRes);
	bool InitializeContext(char* windowTitle);
	bool Update();

	Renderer(void);
	~Renderer(void);
};

