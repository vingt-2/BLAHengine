#pragma once

#include "Graphics.h"
#include "std.h"
#include "Vector3f.h"
#include "Transform.h"
#include "SharedRessources.h"

class MeshRenderer
{
public:
	Transform* modelTransform;

	vector<vec3> meshVertices;
	vector<vec3> meshNormals;
	vector<vec2> meshUVs;

	vector<vector<GLuint>> vboIDVector;

	GLuint vertexArrayID;
	GLuint textureID;
	GLuint programID;
	GLuint matrixID;
	GLuint renderType;

	MeshRenderer(Transform* modelTransform);
	~MeshRenderer(void);

	bool AssignMaterial(const char* name);
	bool AssignTexture(const char* name);

	bool Draw(mat4 projection, mat4 view);
	bool GenerateArrays();
	void GenerateVertexArray(GLuint attributeNumber);
	void GenerateUVArray(GLuint attributeNumber);
	bool CleanUp();

	string ToString(void);
};

