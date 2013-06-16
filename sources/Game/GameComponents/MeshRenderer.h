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

	GLuint m_renderType;

	// Light Vector
	vec3 m_directionalLight;

	//Class functions

	MeshRenderer(Transform* modelTransform);
	~MeshRenderer(void);

	bool AssignMaterial(const char* name);
	bool LoadTextureSample(const char* textureRessourceName,const char* sampleName);

	bool Draw(mat4 projection, mat4 view);

	// OpenGL Buffer related Code:
	bool GenerateArrays(); // <<---- Careful, hardcoded stuff (generates vertices/uv/normals objects for each available info)
	void GenerateVertexArrayID();
	template<typename objectType>
	void GenerateBufferObject(const objectType* buffer, GLuint bufferSize, GLuint elementsPerObject, GLuint attributeNumber)
	{
		glBindVertexArray(m_vertexArrayID);

		// Load it into a VBO
		GLuint bufferObjectID;
		glGenBuffers(1, &bufferObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObjectID);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, &(buffer[0]), GL_STATIC_DRAW);


		glEnableVertexAttribArray(attributeNumber);
		glBindBuffer(GL_ARRAY_BUFFER,bufferObjectID);
		glVertexAttribPointer
		(
			attributeNumber,			// attribute
			elementsPerObject,			// size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			0,							// stride
			(void*)0					// array buffer offset
		);

		glBindVertexArray (0);

		pair<GLuint,pair<GLuint,GLuint>> bufferObject = pair<GLuint,pair<GLuint,GLuint>> (bufferObjectID,pair<GLuint,GLuint>(attributeNumber,elementsPerObject));

		m_vboIDVector.push_back(bufferObject);
	}

	bool CleanUp();
	void CleanUpVBOs();
	void DestroyVertexArrayID();

	string ToString(void);
	bool IsMeshValid();

private:
	//private members
	vector<pair<GLuint,GLuint>> m_activeTextures;
	
	// ID of an openGL object (VAO) that lists VBOs 
	GLuint m_vertexArrayID;
	GLuint m_sizeOfVertexArray;
	// Keeps track of the VBOs we've generated and added to our VAO
	vector<pair<GLuint,pair<GLuint,GLuint>>> m_vboIDVector;

	// All our textures samplers passed uniform to the shader
	vector<pair<GLuint,GLuint>> m_textureSamplersVector;

	// Constant shader Infos
	GLuint m_programID;
	GLuint m_matrixID;

	//Private functions

};

