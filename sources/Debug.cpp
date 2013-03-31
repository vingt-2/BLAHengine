#include "Debug.h"


Debug::Debug(void):
	gizmoVector( vector<MeshRenderer*>() )
{
}


Debug::~Debug(void)
{
}

void Debug::DrawRay(vec3 origin,vec3 destination)
{
	Transform* transform = new Transform();
	MeshRenderer* ray = new MeshRenderer(transform);
	ray->meshVertices.push_back(origin);
	ray->meshVertices.push_back(destination);
	ray->LoadShaders( "../resources/shaders/Vertex_Shader.glsl", "../resources/shaders/Fragment_Shader.glsl" );
	ray->GenerateArrays();
	ray->renderType = GL_LINES;
	gizmoVector.push_back(ray);
}