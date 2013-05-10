#pragma once

#include "Graphics.h"
#include "std.h"
#include "Vector3f.h"
#include "Transform.h"
#include "SharedRessources.h"

class MeshRenderer
{
public:

	//Class Variables
	Transform* modelTransform;

	vector<vec3> meshVertices;
	vector<vec3> meshNormals;
	vector<vec2> meshUVs;

	vector<pair<GLuint,pair<GLuint,GLuint>>> vboIDVector;

	GLuint vertexArrayID;
	GLuint textureID;
	GLuint programID;
	GLuint matrixID;
	GLuint renderType;

	//Class functions

	MeshRenderer(Transform* modelTransform);
	~MeshRenderer(void);

	bool AssignMaterial(const char* name);
	bool AssignTexture(const char* name);

	bool Draw(mat4 projection, mat4 view);
	bool GenerateArrays();

	template<typename objectType>
	void GenerateBufferObject(const objectType* buffer, GLuint bufferSize, GLuint elementsPerObject, GLuint attributeNumber)
	{
		glBindVertexArray(vertexArrayID);

		// Load it into a VBO
		GLuint bufferObjectID;
		glGenBuffers(1, &bufferObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObjectID);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, &(buffer[0]), GL_STATIC_DRAW);

		pair<GLuint,pair<GLuint,GLuint>> bufferObject = pair<GLuint,pair<GLuint,GLuint>> (bufferObjectID,pair<GLuint,GLuint>(attributeNumber,elementsPerObject));

		vboIDVector.push_back(bufferObject);
	}

	bool CleanUp();

	string ToString(void);

private:
	//Public Variables


	//Private functions
	bool IsMeshValid();

};

