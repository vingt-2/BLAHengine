#pragma once
#include "Renderer.h"
#include "../Game/GameComponents/MeshRenderer.h"

class GL33RenderObject : public RenderObject
{
public:

	GL33RenderObject();
	~GL33RenderObject();

	Transform* m_modelTransform;

	const vector<unsigned int>* m_toMeshTriangles;
	const vector<vec3>* m_toMeshVertices;
	const vector<vec3>* m_toMeshNormals;
	const vector<vec3>* m_toMeshTangents;
	const vector<vec3>* m_toMeshBiTangents;
	const vector<vec2>* m_toMeshUVs;

	vector <pair<GLuint, GLuint> > m_activeTextures;

	// ID of an openGL object (VAO) that lists VBOs 
	GLuint m_elementBufferId;
	GLuint m_vertexArrayID;
	GLuint m_sizeOfVertexArray;
	// Keeps track of the VBOs we've generated and added to our VAO
	vector<pair<GLuint, pair<GLuint, GLuint> > > m_vboIDVector;

	// All our textures samplers passed uniform to the shader
	vector<pair<GLuint, GLuint> > m_textureSamplersVector;

	// Constant shader Infos
	GLuint m_programID;
	GLuint m_matrixID;

	GLuint m_renderType;

private:
};

class GL33Renderer : public Renderer
{
public:
	bool Update();
	
	GL33Renderer(char* windowTitle,bool isFullScreen);
	GL33Renderer(char* windowTitle,bool isFullScreen, vec2 renderSize);
	~GL33Renderer();

	RenderObject* LoadRenderObject(const MeshRenderer& meshRenderer, int type);

protected:
	GLFWwindow* InitializeContext(char* windowTitle);
	bool GenerateArrays(GL33RenderObject& object);
	void GenerateVertexArrayID(GL33RenderObject& object);

	template<typename objectType>
	void GenerateBufferObject(GL33RenderObject& object, const objectType* buffer, GLuint bufferSize, GLuint elementsPerObject, GLuint attributeNumber);

	void GenerateElementBuffer(GL33RenderObject& object, GLuint elementBufferId);

	bool CleanUp(GL33RenderObject& object);
	void CleanUpVBOs(GL33RenderObject& object);
	bool AssignMaterial(GL33RenderObject& object, string materialName);
	bool LoadTextureSample(GL33RenderObject& object, string textureName, string sampleName);
	void DestroyVertexArrayID(GL33RenderObject& object);

	bool Draw(GL33RenderObject& object);
};