#pragma once

#include "../../Std/Graphics.h"
#include "../../Std/std.h"
#include "./Transform.h"
#include "../../Engine/SharedRessources.h"

class MeshRenderer
{
public:

	//Class Variables
	Transform* m_modelTransform;

	vector<vec3> m_meshVertices;
	vector<vec3> m_meshNormals;
	vector<vec2> m_meshUVs;

	vector<pair<GLuint,pair<GLuint,GLuint>>> m_vboIDVector;

	GLuint m_vertexArrayID;
	GLuint m_textureID;
	GLuint m_programID;
	GLuint m_matrixID;
	GLuint m_renderType;
	


	// diffuse and shit
	GLuint m_diffuseTexture;
	
	vec3 m_directionalLight;

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
		glBindVertexArray(m_vertexArrayID);

		// Load it into a VBO
		GLuint bufferObjectID;
		glGenBuffers(1, &bufferObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObjectID);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, &(buffer[0]), GL_STATIC_DRAW);

		pair<GLuint,pair<GLuint,GLuint>> bufferObject = pair<GLuint,pair<GLuint,GLuint>> (bufferObjectID,pair<GLuint,GLuint>(attributeNumber,elementsPerObject));

		m_vboIDVector.push_back(bufferObject);
	}

	bool CleanUp();

	string ToString(void);

private:
	//private members

	vector<pair<GLuint,GLuint>> m_activeTextures;


	//Private functions
	bool IsMeshValid();

};

