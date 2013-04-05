#pragma once
#include "Graphics.h"
#include "MeshRenderer.h"
class Debug
{
public:

	vector<MeshRenderer*> gizmoVector;


	void DrawLine(const vec3 origin,const vec3 destination);
	void DrawLine(const vec3 origin,const vec3 destination,const vec4 color);
	void DrawRay(const vec3 origin,const vec3 direction,const GLfloat length, const vec4 color);
	void DrawRay(const vec3 origin,const vec3 direction,const GLfloat length);

	void DrawGrid(int size, const vec4 color);
	void DrawBasis(Transform* transform,GLfloat opacity);

	Debug(void);
	~Debug(void);

private:

	void GenerateLineMesh(const vec3 origin,const vec3 destination,const vec4* colorBuffer);

};

