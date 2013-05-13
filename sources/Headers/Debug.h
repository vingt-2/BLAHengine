#pragma once
#include "Graphics.h"
#include "MeshRenderer.h"

/*
 TODO: 
	SEPARATE render DEBUG, for a specific CONTEXT (when added multiple windows and context)
	And Common debug asserts
*/

class Debug
{
public:

	vector<MeshRenderer*> gizmoVector;

	//Render Debug:
	void DrawLine(const vec3 origin,const vec3 destination);
	void DrawLine(const vec3 origin,const vec3 destination,const vec4 color);
	void DrawRay(const vec3 origin,const vec3 direction,const GLfloat length, const vec4 color);
	void DrawRay(const vec3 origin,const vec3 direction,const GLfloat length);

	void DrawGrid(int size, const vec4 color);
	void DrawBasis(Transform* transform,GLfloat opacity);

	//Common debug::
	static void Debug::OutputToDebug(char* debug);

	Debug(void);
	~Debug(void);

private:

	void GenerateLineMesh(const vec3 origin,const vec3 destination,const vec4* colorBuffer);

};

