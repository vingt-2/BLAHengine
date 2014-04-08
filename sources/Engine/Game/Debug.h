#pragma once
#include "../Game/GameComponents/MeshRenderer.h"

/*
 TODO: 
	SEPARATE render m_debug, for a specific CONTEXT (when added multiple windows and context)
	And Common m_debug asserts
*/

class Debug
{
public:

	vector<MeshRenderer*> m_gizmoVector;

	//Render m_debug:
	void DrawLine(const vec3 origin,const vec3 destination);
	void DrawLine(const vec3 origin,const vec3 destination,const vec4 color);
	void DrawRay(const vec3 origin,const vec3 direction,const GLfloat length, const vec4 color);
	void DrawRay(const vec3 origin,const vec3 direction,const GLfloat length);

	void DrawGrid(int size, const vec4 color);
	void DrawBasis(Transform* transform,GLfloat opacity);

	//Common m_debug::
	static void OutputToDebug(char* m_debug);

	Debug(void);
	~Debug(void);

private:

	void GenerateLineMesh(const vec3 origin,const vec3 destination,const vec4* colorBuffer);

};

