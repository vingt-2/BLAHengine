#pragma once
#include "../../Common/StdInclude.h"
#include "../Assets/AssetsManager.h"
#include "../Game/GameComponents/MeshRenderer.h"
#include "Renderer.h"
#include "GL33Shader.h"

namespace BLAengine 
{
	class BLACORE_API GL33Resources
	{
	public:
		//TODO: Add filtering options
		bool GLLoadTexture(std::string resourcePath, Texture2D texture);
		bool GLLoadShaderProgram(GL33Shader& shader);
		bool GLLoadSystemShaders();

		GL33SystemShaders m_systemShaders;

		std::map<std::string, GLuint> m_glLoadedTextureIds;
		std::map<std::string, GL33Shader> m_glLoadedProgramsIds;
	};


	class BLACORE_API GBuffer
	{
	public:

		ivec2 m_GbufferSize;

		GLuint m_frameBufferObject;

		GLuint m_diffuseTextureTarget;
		GLuint m_normalsTextureTarget;
		GLuint m_worldPosTextureTarget;
		GLuint m_texCoordsTextureTarget;
		GLuint m_depthTextureTarget;

		GLuint m_displayTextureTarget;

		GLuint m_geometryPassPrgmID;
		GLuint m_drawSphereStencilPgrmID;

		bool InitializeGBuffer();
		void SetupGeomPassMaterials(GLuint prgrmId);
		void DeleteGBufferResources();
	};

	class BLACORE_API ScreenSpaceQuad
	{
	public:

		bool m_isInit = false;
		GLuint m_geomBuffer;
		GLuint m_vao;
	};

	class BLACORE_API PointLightSphere
	{
	public:

		bool m_isInit = false;
		GLuint m_geomBuffer;
		GLuint m_elementBuffer;
		GLuint m_vao;

		GLuint m_size;
	};

	class BLACORE_API GL33RenderObject : public RenderObject
	{
	public:

		GL33RenderObject();
		~GL33RenderObject();

		mat4* m_modelTransform;

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

	class BLACORE_API GL33Renderer : public virtual Renderer
	{
	public:

		void InitializeRenderer(RenderWindow* window);
		bool Update();

		RenderObject* LoadRenderObject(const MeshRenderer& meshRenderer, int type);
		bool	CancelRender(const MeshRenderer& object);
		bool	LoadDebugLines(pair<vector<vec3>, vector<vec3>>& debugLinesMesh);

		RenderWindow* GetWindow() const { return m_renderWindow; }
		void		ViewportResize(int width, int height);

		GL33Renderer();
		~GL33Renderer();

		// Debug Vignette;
		GLuint DrawColorBufferPrgmID;
		GLuint DrawDepthBufferPrgmID;
		//

		vector<DirectionalLightRender> m_directionalLightsVector;
		vector<PointLightRender> m_pointLightsVector;

		GBuffer m_GBuffer;
		ScreenSpaceQuad m_screenSpaceQuad;
		PointLightSphere m_pointLightSphereMesh;

		GL33Resources m_glResources;

		// External References 
		AssetManager* m_assetManager;
		
		bool debug_renderGBuffer;

		bool m_renderDebug;

		struct debugLinesInfo
		{
			GLuint vertBuffer;
			GLuint colorBuffer;
			GLuint vao;
			GLuint size;
		} m_debugLinesInfo;

		GLuint m_debugRayPgrmID;

		// MOVE?
		bool SetupDirectionalShadowBuffer(DirectionalShadowRender& shadowRender);
		void SetupPointLightRenderSphere(vector<vec3> sphereMeshVertices, vector<GLuint> indices);

	protected:

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
		void CleanUpFrameDebug();

		void RenderGBuffer();

		void DrawDisplayBuffer();

		//void RenderDefferedLights();

		void RenderDebugLines();
		void RenderDebug()
		{
			if (m_renderDebug)
			{
				RenderDebugLines();
			}
		}

		void DrawColorBufferOnScreen(ivec2 topLeft, ivec2 bottomRight, GLuint textureTarget);
		void DrawDepthBufferOnScreen(ivec2 topLeft, ivec2 bottomRight, GLuint textureTarget);

		void DrawDirectionalLight(DirectionalLightRender directionalLight);

		void DrawPointLight(PointLightRender pointLight);

		bool RenderDirectionalShadowMap(DirectionalShadowRender& shadowRender);

		void SetupScreenSpaceRenderQuad();
	};
}