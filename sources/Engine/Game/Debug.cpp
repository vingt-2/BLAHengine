#include "./Debug.h"


Debug::Debug(RenderingManager* manager)
{
	this->m_debugRenderManager = manager;
}


Debug::~Debug(void)
{
}

void Debug::DrawLine(const vec3 origin,const vec3 destination)
{
	vec4 colorBuffer[2] = {vec4(origin,1.f),vec4(destination,1.f)};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::DrawLine(const vec3 origin,const vec3 destination,const vec4 color)
{
	vec4 colorBuffer[2] = {color,color};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::DrawRay(const vec3 origin,const vec3 direction,const GLfloat length,const vec4 color)
{
	vec3 destination = origin + length* direction;
	vec4 colorBuffer[2] = {color,color};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::DrawRay(const vec3 origin,const vec3 direction,const GLfloat length)
{
	vec3 destination = origin + length* direction;
	vec4 colorBuffer[2] = {vec4(origin,1.f),vec4(destination,1.f)};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::DrawGrid(int size,const vec4 color)
{
	for (int i=-size/2; i<=size/2; i++)
	{
		DrawLine(vec3(size/2,0,i),vec3(-size/2,0,i),color);
	}
	for (int i=-size/2; i<=size/2; i++)
	{
		DrawLine(vec3(i,0,size/2),vec3(i,0,-size/2),color);
	}
}

void Debug::GenerateLineMesh(const vec3 origin, const vec3 destination, const vec4* colorBuffer)
{
	Transform* transform = new Transform();
	TriangleMesh* ray = new TriangleMesh();
	ray->m_vertexPos.push_back(origin);
	ray->m_vertexPos.push_back(destination);
	//ray->m_meshTriangles.push_back(0);
	//ray->m_meshTriangles.push_back(1);
	MeshRenderer* rayRenderer = new MeshRenderer(transform);
	rayRenderer->AssignMaterial("debugShader");
	rayRenderer->m_renderType = GL_LINES;
	GameChar* lineMesh = new GameChar();
	lineMesh->m_meshRenderer = rayRenderer;
	lineMesh->m_transform = transform;

	m_debugRenderManager->RequestRenderTicket(*lineMesh);
}

void Debug::DrawBasis(Transform* transform,GLfloat opacity)
{
	DrawRay((transform->LocalPositionToWorld(vec3(0,0,0))),normalize((transform->LocalDirectionToWorld(vec3(1,0,0)))),1,vec4(1,0,0,opacity));
	DrawRay((transform->LocalPositionToWorld(vec3(0,0,0))),normalize((transform->LocalDirectionToWorld(vec3(0,1,0)))),1,vec4(0,1,0,opacity));
	DrawRay((transform->LocalPositionToWorld(vec3(0,0,0))),normalize((transform->LocalDirectionToWorld(vec3(0,0,1)))),1,vec4(0,0,1,opacity));
}

void Debug::OutputToDebug(char* m_debug)
{
	
	// WINDOWS SPECIFIC WINDOW HANDLER
	
	//	OutputDebugString(debug);
	//	OutputDebugString("\n");
}
