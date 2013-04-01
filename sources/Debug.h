#pragma once
#include "Graphics.h"
#include "MeshRenderer.h"
class Debug
{
public:

	vector<MeshRenderer*> gizmoVector;


	void DrawRay(vec3 origin,vec3 destination,vec3 Color);
	void DrawLine(vec3 origin,vec3 direction,vec3 length,vec3 Color);


	Debug(void);
	~Debug(void);
};

