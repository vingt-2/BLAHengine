#pragma once
#include "../Game/GameObjects/GameChar.h"
#include "../Game/RenderingManager.h"
#include "..\..\Common\StdInclude.h"

/*
 TODO: 
	SEPARATE render m_debug, for a specific CONTEXT (when added multiple windows and context)
	And Common m_debug asserts
*/

class BLACORE_API Debug
{
public:

	Debug(DebugRenderingManager* renderManager);
	~Debug();

	DebugRenderingManager* m_debugRenderManager;

	bool m_drawDebugRays;

	void Update();

	//Render m_debug:
	void DrawLine(const vec3 origin,const vec3 destination);
	void DrawLine(const vec3 origin,const vec3 destination,const vec3 color);
	void DrawRay(Ray ray, const vec3 color);
	void DrawRay(Ray ray);

	void DrawGrid(int size, float spacing, const vec3 color);
	void DrawBasis(Transform* transform, float opacity);

	//Common m_debug::
	static void OutputToDebug(char* m_debug);

private:
	pair<vector<vec3>,vector<vec3>> m_lineMeshVertsAndColor;
};

