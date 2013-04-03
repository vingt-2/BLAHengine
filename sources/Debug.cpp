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
	vec3 colorBuffer[2] = {origin,destination};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::DrawLine(const vec3 origin,const vec3 destination,const vec3 color)
{
	vec3 colorBuffer[2] = {color,color};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::DrawRay(const vec3 origin,const vec3 direction,const GLfloat length,const vec3 color)
{
	vec3 destination = origin + length* direction;
	vec3 colorBuffer[2] = {color,color};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::DrawRay(const vec3 origin,const vec3 direction,const GLfloat length)
{
	vec3 destination = origin + length* direction;
	vec3 colorBuffer[2] = {origin,destination};
	GenerateLineMesh(origin,destination,colorBuffer);
}

void Debug::GenerateLineMesh(const vec3 origin, const vec3 destination, const vec3* colorBuffer)
{
	Transform* transform = new Transform();
	MeshRenderer* ray = new MeshRenderer(transform);
	ray->meshVertices.push_back(origin);
	ray->meshVertices.push_back(destination);
	ray->AssignMaterial("debugShader");
	ray->GenerateArrays();

	glBindVertexArray(ray->vertexArrayID);

	GLuint colorBufferID;
	glGenBuffers(1, &colorBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), colorBuffer, GL_STATIC_DRAW);

	pair<GLuint,GLuint> colorVBO = pair<GLuint,GLuint> (colorBufferID,1);

	ray->vboIDVector.push_back(colorVBO);
	ray->renderType = GL_LINES;

	gizmoVector.push_back(ray);
}
