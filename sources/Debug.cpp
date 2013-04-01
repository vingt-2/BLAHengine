#include "Debug.h"


Debug::Debug(void):
	gizmoVector( vector<MeshRenderer*>() )
{
}


Debug::~Debug(void)
{
}

void Debug::DrawRay(vec3 origin,vec3 destination,vec3 color)
{
	Transform* transform = new Transform();
	MeshRenderer* ray = new MeshRenderer(transform);
	ray->meshVertices.push_back(origin);
	ray->meshVertices.push_back(destination);
	ray->LoadShaders( "../resources/shaders/Debug_Vertex.glsl", "../resources/shaders/Debug_Fragment.glsl" );
	ray->GenerateArrays();

	vec3 colors[2] = { color, color };

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, 1, &colors[0], GL_STATIC_DRAW);

	vector<GLuint> colorVBO = vector<GLuint>();
	colorVBO.push_back(colorBuffer);
	colorVBO.push_back(1);

	ray->vboIDVector.push_back(colorVBO);
	ray->renderType = GL_LINES;
	gizmoVector.push_back(ray);
}