// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "RenderBackend.h"
#include "Assets/AssetsManager.h"

#include "Renderer.h"
#include "GL33Shader.h"

namespace BLA
{
    class DirectionalShadowRender;

    class BLACORE_API GL33Resources
    {
    public:
        //TODO: Add filtering options
        bool GLLoadTexture(blaString resourcePath, const Texture2D& texture);
        bool GLLoadShaderProgram(GL33Shader& shader);
        bool GLLoadSystemShaders();

        GL33SystemShaders m_systemShaders;

        blaMap<blaString, GLuint> m_glLoadedTextureIds;
        blaMap<blaString, GL33Shader> m_glLoadedProgramsIds;
    };

    class BLACORE_API GBuffer
    {
    public:

        blaIVec2 m_GbufferSize;

        GLuint m_frameBufferObject;

        GLuint m_diffuseTextureTarget = 0xffffffff;
        GLuint m_normalsTextureTarget = 0xffffffff;
        GLuint m_worldPosTextureTarget = 0xffffffff;
        GLuint m_texCoordsTextureTarget = 0xffffffff;;
        GLuint m_depthTextureTarget = 0xffffffff;;

        GLuint m_displayTextureTarget = 0xffffffff;

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

        blaIndex m_size;
    };

    class BLACORE_API GL33RenderObject : public RenderObject
    {
    public:

        GL33RenderObject();
        ~GL33RenderObject();

        const blaMat4* m_modelTransform;

        const blaVector<blaU32>* m_toMeshTriangles;
        const blaVector<blaVec3>* m_toMeshVertices;
        const blaVector<blaVec3>* m_toMeshNormals;
        const blaVector<blaVec3>* m_toMeshTangents;
        const blaVector<blaVec3>* m_toMeshBiTangents;
        const blaVector<blaVec2>* m_toMeshUVs;
    	
        blaVector<blaPair<const Material*, blaU32>> m_materialAndIndex;

        // ID of an openGL object (VAO) that lists VBOs 
        GLuint m_elementBufferId;
        GLuint m_vertexArrayID;
        GLuint m_sizeOfVertexArray;

        // Keeps track of the VBOs we've generated and added to our VAO
        blaVector<blaPair<GLuint, blaPair<GLuint, GLuint> > > m_vboIDVector;

        // Constant shader Infos
        GLuint m_programID;
        GLuint m_matrixID;

        GLuint m_renderType;
    };

    class BLACORE_API GL33Renderer : public virtual Renderer
    {
    public:

        void InitializeRenderer(RenderWindow* window, RenderingManager* renderingManager, DebugRenderingManager* debugRenderManager);
        void SwitchRenderingManager(RenderingManager* renderingManager);
        bool Update();

        RenderObject* LoadRenderObject(const MeshRendererComponent& meshRenderer, int type);
        bool    CancelRender(const MeshRendererComponent& object);
        bool    LoadDebugLines();
        bool	LoadDebugMeshes();

        void CleanUpPools() override;

        RenderWindow* GetWindow() const { return m_renderWindow; }
        void        ViewportResize(int width, int height);
        Ray        ScreenToRay(blaVec2 screenSpaceCoord);

        GL33Renderer();
        ~GL33Renderer() override;

        // Debug Vignette;
        GLuint m_drawColorBufferPrgmID;
        GLuint m_drawNormalsBufferPrgmID;
        GLuint DrawDepthBufferPrgmID;
        //

        blaVec3 m_clearColor;

        GBuffer m_GBuffer;
        ScreenSpaceQuad m_screenSpaceQuad;
        PointLightSphere m_pointLightSphereMesh;

        GL33Resources m_glResources;

        // External References 
        AssetManager* m_assetManager;

        bool m_debugDrawGBuffer;

        bool m_renderDebug;

        struct debugLinesInfo
        {
            GLuint vertBuffer;
            GLuint colorBuffer;
            GLuint vao;
            blaIndex size;
        } m_debugLinesInfo;

        GLuint m_debugRayPgrmID;

        struct debugMeshesInfo
        {
            GLuint vertBuffer;
            GLuint colorBuffer;
            GLuint vao;
            blaIndex size;
        } m_debugMeshesInfo;

        GLuint m_debugMeshesPgrmID;

        // MOVE?
        bool SetupDirectionalShadowBuffer(DirectionalShadowRender& shadowRender);
        void SetupPointLightRenderSphere();

        GLuint GetDisplayBufferTexture() const { return m_GBuffer.m_displayTextureTarget; }

        virtual void SetRenderSize(blaIVec2 renderSize);

    protected:

        int SynchWithRenderManager();

        bool GenerateArrays(GL33RenderObject& object);
        void GenerateVertexArrayID(GL33RenderObject& object);
        template<typename objectType>
        void GenerateBufferObject(GL33RenderObject& object, const objectType* buffer, GLuint bufferSize, GLuint elementsPerObject, GLuint attributeNumber);
        void GenerateElementBuffer(GL33RenderObject& object, GLuint elementBufferId);

        bool CleanUp(GL33RenderObject& object);
        void CleanUpVBOs(GL33RenderObject& object);
        const Material* LoadMaterial(GL33RenderObject& object, blaString materialName);
        bool LoadTextureSample(GL33RenderObject& object, blaString textureName, blaString sampleName);
        void DestroyVertexArrayID(GL33RenderObject& object);
        void CleanUpFrameDebug();

        void RenderGBuffer();

        void DrawDisplayBuffer();

        void RenderDebugLines();
        void RenderDebugMeshes();
        void RenderDebug();

        void DrawColorBufferOnScreen(glm::vec2 topLeft, glm::vec2 bottomRight, GLuint textureTarget);
        void DrawNormalBufferOnScreen(glm::vec2 topLeft, glm::vec2 bottomRight, GLuint textureTarget);
        void DrawDepthBufferOnScreen(glm::vec2 topLeft, glm::vec2 bottomRight, GLuint textureTarget);

        void DrawDirectionalLight(DirectionalLightRender* directionalLight);

        void DrawPointLight(PointLightRender* pointLight);

        bool RenderDirectionalShadowMap(DirectionalShadowRender& shadowRender);

        void SetupScreenSpaceRenderQuad();
    };
}