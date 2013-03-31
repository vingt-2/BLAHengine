#pragma once
#include "Graphics.h"
#include "MeshRenderer.h"
class Debug
{
public:

	vector<MeshRenderer*> gizmoVector;


	void DrawRay(vec3 origin,vec3 destination);


	Debug(void);
	~Debug(void);
};

