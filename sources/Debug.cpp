#include "Debug.h"


Debug::Debug(void):
	gizmoVector( vector<MeshRenderer*>() )
{
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
	MeshRenderer* ray = new MeshRenderer(transform);
	ray->meshVertices.push_back(origin);
	ray->meshVertices.push_back(destination);
	ray->AssignMaterial("debugShader");
	ray->GenerateArrays();

	ray->GenerateBufferObject<vec4>(colorBuffer,2*sizeof(vec4),4,1);
	
	ray->renderType = GL_LINES;

	gizmoVector.push_back(ray);
}

void Debug::DrawBasis(Transform* transform,GLfloat opacity)
{
	DrawLine((transform->LocalPositionToWorld(vec3(0,0,0))),transform->LocalPositionToWorld(vec3(100,0,0)),vec4(1,0,0,opacity));
	DrawLine((transform->LocalPositionToWorld(vec3(0,0,0))),transform->LocalPositionToWorld(vec3(0,100,0)),vec4(0,1,0,opacity));
	DrawLine((transform->LocalPositionToWorld(vec3(0,0,0))),transform->LocalPositionToWorld(vec3(0,0,100)),vec4(0,0,1,opacity));
}

