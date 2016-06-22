#pragma once
#include "Renderer.h"
#include "../Game/GameComponents/MeshRenderer.h"

class GBuffer
{
public:

	ivec2 m_GbufferSize;

	GLuint m_frameBufferObject;

	GLuint m_diffuseTextureTarget;
	GLuint m_normalsTextureTarget;
	GLuint m_worldPosTextureTarget;
	GLuint m_texCoordsTextureTarget;
	GLuint m_depthTextureTarget;

	GLuint m_geometryPassPrgmID;

	bool InitializeGBuffer();
	void SetupGeomPassMaterials(GLuint prgrmId);
	void DeleteGBufferResources();
};

class ScreenSpaceQuad
{
public:

	bool m_isInit = false;
	GLuint m_geomBuffer;
	GLuint m_vao;
};

class GL33RenderObject : public RenderObject
{
public:

	GL33RenderObject();
	~GL33RenderObject();

	Transform* m_modelTransform;

	const vector<uint32>* m_toMeshTriangles;
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

	RenderObject* LoadRenderObject(const MeshRenderer& meshRenderer, int type);
	bool		  CancelRender(const MeshRenderer& object);

	GLFWwindow* GetWindow() const { return m_glfwWindow; }
	void		WindowResize(GLFWwindow* window, int width, int height);
	vec2		GetCursorPosition();

	GL33Renderer(char* windowTitle, bool isFullScreen);
	~GL33Renderer();

	// Debug Vignette;
	GLuint DrawColorBufferPrgmID;
	GLuint DrawDepthBufferPrgmID;
	//

	vector<DirectionalLightRender> m_directionalLightsVector;
	GBuffer m_GBuffer;
	ScreenSpaceQuad m_screenSpaceQuad;

	bool debug_renderGBuffer = false;
	vector<pair<Ray,vec3>> m_debugRaysQueue;
	GLuint m_debugRayPgrmID;

	// MOVE?
	bool SetupDirectionalShadowBuffer(DirectionalShadowRender& shadowRender);

protected:
	GLFWwindow* InitializeWindowAndContext(char* windowTitle);
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

	void RenderGBuffer();

	void RenderDefferedLights();

	void RenderDebugRays();
	void RenderDebug()
	{
		RenderDebugRays();
	}

	void DrawColorBufferOnScreen(ivec2 topLeft, ivec2 bottomRight, GLuint textureTarget);
	void DrawDepthBufferOnScreen(ivec2 topLeft, ivec2 bottomRight, GLuint textureTarget);

	void DrawDirectionalLight(DirectionalLightRender directionalLight);

	bool RenderDirectionalShadowMap(DirectionalShadowRender& shadowRender);

	void SetupScreenSpaceRenderQuad();
};