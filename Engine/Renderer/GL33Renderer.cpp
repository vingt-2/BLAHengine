#include "GL33Renderer.h"

#include "System/Console.h"
#include "Renderer/MeshRendererComponent.h"
#include "Renderer/PointLightComponent.h"
#include "Renderer/LightRender.h"
#include "Core/TransformComponent.h"
#include "Geometry/PrimitiveGeometry.h"

#pragma optimize("", off)

using namespace BLA;

TriangleMesh g_PointLightRenderSphere = PrimitiveGeometry::MakeSphere(1.f);

GL33Renderer::GL33Renderer() :
    m_debugDrawGBuffer(false),
    m_renderDebug(true),
    m_clearColor(blaVec3(0.3, 0.3, 0.3))
{}

GL33Renderer::~GL33Renderer()
{
    CleanUpPools();
}

void GL33Renderer::ViewportResize(int width, int height)
{
    m_renderSize = blaIVec2(width, height);

    m_mainRenderCamera.SetPerspective(m_renderSize);

    m_GBuffer.DeleteGBufferResources();
    m_GBuffer.m_GbufferSize = m_renderSize;
    m_GBuffer.InitializeGBuffer();
}

Ray GL33Renderer::ScreenToRay(blaVec2 screenSpaceCoord)
{
    blaVec3 rayDirection = blaVec3(1.f);
    rayDirection.x = (2.0f * screenSpaceCoord.x - 1.f) / this->m_mainRenderCamera.m_perspectiveProjection[0][0];

    rayDirection.y = -(2.0f * screenSpaceCoord.y - 1.f) / this->m_mainRenderCamera.m_perspectiveProjection[1][1];

    blaMat4 viewTransformMatrix;
    this->m_mainRenderCamera.m_attachedCamera->m_viewTransform.GetScaledTransformMatrix(viewTransformMatrix);

    blaMat4 inverseView = inverse(viewTransformMatrix);

    blaVec4 direction = (inverseView * blaVec4(rayDirection, 0));
    rayDirection = normalize(blaVec3(direction.x, direction.y, direction.z));

    blaVec3 rayOrigin = blaVec3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);

    return Ray(rayOrigin, -1.f * rayDirection, MAX_NORMAL_FLOAT);
}

GL33RenderObject::GL33RenderObject() :
    m_vboIDVector(blaVector<blaPair<GLuint, blaPair<GLuint, GLuint> > >())
{}

GL33RenderObject::~GL33RenderObject() {}

/*
    Rendering code below
*/
bool GL33Renderer::Update()
{
    int width, height;

    if (!m_renderToFrameBufferOnly)
    {
        m_renderWindow->GetSize(width, height);

        if (width != m_renderSize.x || height != m_renderSize.y)
        {
            ViewportResize(width, height);
        }
    }

    SynchWithRenderManager();

    //Set OpenGL to this context.
    m_renderWindow->MakeGLContextCurrent();

    m_mainRenderCamera.Update();

    RenderGBuffer();

    // Draw vignettes with buffer results (from Gbuffer pass)
    if (m_debugDrawGBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);
        GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
        glDrawBuffers(1, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        DrawColorBufferOnScreen(glm::vec2(0, 0), glm::vec2(m_renderSize.x / 2, m_renderSize.y / 2), m_GBuffer.m_diffuseTextureTarget);
        DrawColorBufferOnScreen(glm::vec2(m_renderSize.x / 2, 0), glm::vec2(m_renderSize.x, m_renderSize.y / 2), m_GBuffer.m_worldPosTextureTarget);
        DrawNormalBufferOnScreen(glm::vec2(0, m_renderSize.y / 2), glm::vec2(m_renderSize.x / 2, m_renderSize.y), m_GBuffer.m_normalsTextureTarget);
        /* if (m_directionalLightPool.size() > 0)
         {
             DirectionalLightRender* pFirstDirLight = nullptr;
             for (auto dlptr : m_directionalLightPool)
             {
                 if (dlptr.second != nullptr)
                 {
                     pFirstDirLight = dlptr.second;
                     break;
                 }
             }
             RenderDirectionalShadowMap(pFirstDirLight->m_shadowRender);
             DrawDepthBufferOnScreen(glm::vec2(m_renderSize.x / 2, m_renderSize.y / 2), glm::vec2(m_renderSize.x, m_renderSize.y), pFirstDirLight->m_shadowRender.m_depthTexture);
         }*/
    //    DrawColorBufferOnScreen(glm::vec2(m_renderSize.x / 2, 0), glm::vec2(m_renderSize.x, m_renderSize.y / 2), m_GBuffer.m_texCoordsTextureTarget);
    //}
    //    else
        
        DrawColorBufferOnScreen(glm::vec2(m_renderSize.x / 2, m_renderSize.y / 2), glm::vec2(m_renderSize.x, m_renderSize.y), m_GBuffer.m_texCoordsTextureTarget);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {

        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);
        GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
        glDrawBuffers(1, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        for (auto ticketedDirLightRender : m_directionalLightPool)
        {
            DirectionalLightRender* dirLightRender = ticketedDirLightRender.second;
            RenderDirectionalShadowMap(dirLightRender->m_shadowRender);
            DrawDirectionalLight(dirLightRender);
        }

        for (auto pointLight : m_pointLightPool)
        {
            DrawPointLight(pointLight.second);
        }

    	
        RenderDebug();

        if (!m_renderToFrameBufferOnly)
        {
            DrawDisplayBuffer();
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
    }

    CleanUpFrameDebug();

    return true;
}

void GL33Renderer::RenderGBuffer()
{
    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

    // Multiple render targets code. This represent the output buffers of the fragment shading
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, DrawBuffers);

    glViewport(0, 0, static_cast<GLsizei>(m_GBuffer.m_GbufferSize.x), static_cast<GLsizei>(m_GBuffer.m_GbufferSize.y));

    // Clear Frame Buffer.
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_GBuffer.m_geometryPassPrgmID);

    for (auto loadedRenderObject : m_meshRenderPool)
    {
        RenderObject* renderObject = loadedRenderObject.second;
        if (GL33RenderObject* gl33RenderObject = dynamic_cast<GL33RenderObject*>(renderObject))
        {
            blaMat4 MVP = m_mainRenderCamera.m_ViewProjection  * (*(gl33RenderObject->m_modelTransform));

            GLuint MVPid = glGetUniformLocation(m_GBuffer.m_geometryPassPrgmID, "MVP");
            glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

            //send modelTransform to shader
            GLuint transformID = glGetUniformLocation(m_GBuffer.m_geometryPassPrgmID, "modelTransform");
            glUniformMatrix4fv(transformID, 1, GL_FALSE, &(*(gl33RenderObject->m_modelTransform))[0][0]);


        	// Render all materials
            for (blaIndex i = 0; i < gl33RenderObject->m_materialAndIndex.size(); ++i)
            {
            	const auto& matAndIndex = gl33RenderObject->m_materialAndIndex[i];
            	
                blaU16 samplerCount = 0;
                for (const auto& sampler : matAndIndex.first->GetSamplers())
                {
                    if (sampler.second == "alphaMap") continue;
                    if (m_glResources.m_glLoadedTextureIds.count(sampler.first))
                    {
                        GLuint glResourceTextureId = m_glResources.m_glLoadedTextureIds[sampler.first];
                        GLuint textureHandleID = glGetUniformLocation(m_GBuffer.m_geometryPassPrgmID, sampler.second.data());

                        // Send Texture Samplers to Shader
                        glActiveTexture(GL_TEXTURE0 + samplerCount);
                        glBindTexture(GL_TEXTURE_2D, glResourceTextureId);
                        glUniform1i(textureHandleID, samplerCount);
                        samplerCount++;
                    }
                    else
                    {
                        Console::LogError("Failed to find instance of texture " + sampler.first + " in GPU memory.");
                    }
                }

                blaIndex drawCallSize;
                if (i < gl33RenderObject->m_materialAndIndex.size() - 1)
                {
                    drawCallSize = 3 * (gl33RenderObject->m_materialAndIndex[i + 1].second - matAndIndex.second);
                }
                else
                {
                    drawCallSize = gl33RenderObject->m_toMeshTriangles->size() - 3 * matAndIndex.second;
                }

                char* offset = (char*)0 + 3 *  matAndIndex.second * sizeof(blaU32);

                glBindVertexArray(gl33RenderObject->m_vertexArrayID);

                // Draw VAO
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(drawCallSize), GL_UNSIGNED_INT, offset); // Draw Triangles
                //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gl33RenderObject->m_toMeshTriangles->size()), GL_UNSIGNED_INT, 0); // Draw Triangles
            	
                glBindVertexArray(0);
            }

            for (blaU32 vboIndex = 0; vboIndex < gl33RenderObject->m_vboIDVector.size(); vboIndex++)
            {
                glDisableVertexAttribArray(vboIndex);
            }
        }
    }

    glUseProgram(0);
}

void GL33Renderer::DrawDisplayBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    DrawColorBufferOnScreen(blaVec2(0, 0), m_renderSize, m_GBuffer.m_displayTextureTarget);
}

RenderObject* GL33Renderer::LoadRenderObject(const MeshRendererComponent& meshRenderer, int type)
{
    GL33RenderObject* object = new GL33RenderObject();
    this->GenerateVertexArrayID(*object);
    const RenderData* renderData = &(meshRenderer.GetMeshAsset()->m_triangleMesh.m_renderData);
    object->m_toMeshTriangles = &(renderData->m_triangleIndices);
    object->m_toMeshVertices = &(renderData->m_vertPos);
    object->m_toMeshNormals = &(renderData->m_vertNormal);
    object->m_toMeshTangents = &(renderData->m_vertTangent);
    object->m_toMeshBiTangents = &(renderData->m_vertBiTangent);
    object->m_toMeshUVs = &(renderData->m_vertUVs);

    object->m_modelTransform = meshRenderer.GetTransformMatrix();

    if (!this->GenerateArrays(*object))
    {
        return nullptr;
    }

    for(auto mat : meshRenderer.GetMeshAsset()->m_triangleMesh.m_materials)
    {
        const Material* matPtr = LoadMaterial(*object, mat.first);
        object->m_materialAndIndex.push_back(std::make_pair(matPtr, static_cast<blaU32>(mat.second)));
    }

	
    object->m_renderType = 4;

    return object;
}

bool GL33Renderer::CancelRender(const MeshRendererComponent& object)
{
    return true;
}

bool GL33Renderer::LoadDebugLines()
{
    for (auto debugLinesMesh : m_debugRenderingManager->m_lineMeshes)
    {
        if (debugLinesMesh.first.size() == 0 || debugLinesMesh.second.size() != debugLinesMesh.first.size())
        {
            m_debugLinesInfo = { 0, 0, 0, 0 };
            return false;
        }

        m_debugLinesInfo.size = debugLinesMesh.first.size();

        glGenBuffers(1, &(m_debugLinesInfo.vertBuffer));
        glBindBuffer(GL_ARRAY_BUFFER, m_debugLinesInfo.vertBuffer);
        glBufferData(GL_ARRAY_BUFFER, m_debugLinesInfo.size * sizeof(blaVec3), &(debugLinesMesh.first[0]), GL_STATIC_DRAW);

        glGenBuffers(1, &(m_debugLinesInfo.colorBuffer));
        glBindBuffer(GL_ARRAY_BUFFER, m_debugLinesInfo.colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, m_debugLinesInfo.size * sizeof(blaVec3), &(debugLinesMesh.second[0]), GL_STATIC_DRAW);

        // 1rst attribute buffer : vertices
        glGenVertexArrays(1, &(m_debugLinesInfo.vao));
        glBindVertexArray(m_debugLinesInfo.vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_debugLinesInfo.vertBuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_debugLinesInfo.colorBuffer);
        glVertexAttribPointer(
            1,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
    }

    return true;
}

bool GL33Renderer::LoadDebugMeshes()
{
    for (auto debugFilledMesh : m_debugRenderingManager->m_filledMeshes)
    {
        if (debugFilledMesh.first.size() == 0 || debugFilledMesh.second.size() != debugFilledMesh.first.size())
        {
            m_debugMeshesInfo = { 0, 0, 0, 0 };
            return false;
        }

        m_debugMeshesInfo.size = debugFilledMesh.first.size();

        glGenBuffers(1, &(m_debugMeshesInfo.vertBuffer));
        glBindBuffer(GL_ARRAY_BUFFER, m_debugMeshesInfo.vertBuffer);
        glBufferData(GL_ARRAY_BUFFER, m_debugMeshesInfo.size * sizeof(blaVec3), &(debugFilledMesh.first[0]), GL_STATIC_DRAW);

        glGenBuffers(1, &(m_debugMeshesInfo.colorBuffer));
        glBindBuffer(GL_ARRAY_BUFFER, m_debugMeshesInfo.colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, m_debugMeshesInfo.size * sizeof(blaVec4), &(debugFilledMesh.second[0]), GL_STATIC_DRAW);

        // 1rst attribute buffer : vertices
        glGenVertexArrays(1, &(m_debugMeshesInfo.vao));
        glBindVertexArray(m_debugMeshesInfo.vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_debugMeshesInfo.vertBuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_debugMeshesInfo.colorBuffer);
        glVertexAttribPointer(
            1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
            4,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
    }

    return true;
}

void GL33Renderer::CleanUpPools()
{
    for (auto entry : m_meshRenderPool)
    {
        RenderObject* renderObject = entry.second;
        if (GL33RenderObject* gl33RenderObject = dynamic_cast<GL33RenderObject*>(renderObject))
        {
            this->CleanUp(*gl33RenderObject);
        }
    }
    m_meshRenderPool.clear();

    for (auto dirLight : m_directionalLightPool)
    {
        delete dirLight.second;
    }

    m_directionalLightPool.clear();
}

bool GL33Renderer::SetupDirectionalShadowBuffer(DirectionalShadowRender& shadowRender)
{
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &(shadowRender.m_shadowBuffer));
    glBindFramebuffer(GL_FRAMEBUFFER, shadowRender.m_shadowBuffer);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &(shadowRender.m_depthTexture));
    glBindTexture(GL_TEXTURE_2D, shadowRender.m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowRender.m_bufferSize, shadowRender.m_bufferSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowRender.m_depthTexture, 0);

    glDrawBuffer(GL_NONE); // No color buffer is drawn to.

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void GL33Renderer::DrawColorBufferOnScreen(glm::vec2 topLeft, glm::vec2 bottomRight, GLuint textureTarget)
{
    if (!m_screenSpaceQuad.m_isInit)
        SetupScreenSpaceRenderQuad();

    glDisable(GL_BLEND);

    //glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    glViewport(static_cast<GLsizei>(topLeft.x), 
        static_cast<GLsizei>(topLeft.y), 
        static_cast<GLsizei>(bottomRight.x - topLeft.x), 
        static_cast<GLsizei>(bottomRight.y - topLeft.y));

    // Use our shader
    glUseProgram(m_drawColorBufferPrgmID);
    GLuint texID = glGetUniformLocation(m_drawColorBufferPrgmID, "colorTexture");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(texID, 0);

    // 1rst attribute buffer : vertices
    glBindVertexArray(m_screenSpaceQuad.m_vao);
    glEnableVertexAttribArray(0);

    // Draw the triangle !
    // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glDisableVertexAttribArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GL33Renderer::DrawNormalBufferOnScreen(glm::vec2 topLeft, glm::vec2 bottomRight, GLuint textureTarget)
{
    if (!m_screenSpaceQuad.m_isInit)
        SetupScreenSpaceRenderQuad();

    glDisable(GL_BLEND);

    //glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    glViewport(static_cast<GLsizei>(topLeft.x),
        static_cast<GLsizei>(topLeft.y),
        static_cast<GLsizei>(bottomRight.x - topLeft.x),
        static_cast<GLsizei>(bottomRight.y - topLeft.y));

    // Use our shader
    glUseProgram(m_drawNormalsBufferPrgmID);
    GLuint texID = glGetUniformLocation(m_drawNormalsBufferPrgmID, "colorTexture");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(texID, 0);

    // 1rst attribute buffer : vertices
    glBindVertexArray(m_screenSpaceQuad.m_vao);
    glEnableVertexAttribArray(0);

    // Draw the triangle !
    // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glDisableVertexAttribArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GL33Renderer::DrawDepthBufferOnScreen(glm::vec2 topLeft, glm::vec2 bottomRight, GLuint textureTarget)
{
    if (!m_screenSpaceQuad.m_isInit)
        SetupScreenSpaceRenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(static_cast<GLsizei>(topLeft.x), static_cast<GLsizei>(topLeft.y), static_cast<GLsizei>(bottomRight.x - topLeft.x), static_cast<GLsizei>(bottomRight.y - topLeft.y));

    // Use our shader
    glUseProgram(DrawDepthBufferPrgmID);
    GLuint texID = glGetUniformLocation(DrawDepthBufferPrgmID, "depthTexture");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(texID, 0);

    glBindVertexArray(m_screenSpaceQuad.m_vao);
    glEnableVertexAttribArray(0);

    // Draw the triangle !
    // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
    glDisable(GL_COMPARE_R_TO_TEXTURE);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glDisableVertexAttribArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GL33Renderer::DrawDirectionalLight(DirectionalLightRender* directionalLight)
{
    if (!m_screenSpaceQuad.m_isInit)
        SetupScreenSpaceRenderQuad();

    OrthographicCamera* shadowCamera = &(directionalLight->m_shadowRender.m_shadowCamera);

    glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

    glDisable(GL_DEPTH_TEST);

    // Multiple render targets code. This represent the output buffers of the fragment shading
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(1, DrawBuffers);

    glViewport(0, 0, m_renderSize.x, m_renderSize.y);

    // Clear Frame Buffer.
    glClearColor(0.3f, 0.3f, 0.3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Use our shader
    GLuint prgmID = m_glResources.m_glLoadedProgramsIds["DirectionalLight"].m_loaded_id;
    glUseProgram(prgmID);
    GLuint diffuseMapID = glGetUniformLocation(prgmID, "diffuseMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_diffuseTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(diffuseMapID, 0);

    GLuint normalMapID = glGetUniformLocation(prgmID, "normalMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_normalsTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 1
    glUniform1i(normalMapID, 1);

    GLuint worldPosMapID = glGetUniformLocation(prgmID, "worldPosMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_worldPosTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 2
    glUniform1i(worldPosMapID, 2);

    GLuint depthMapID = glGetUniformLocation(prgmID, "depthMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_depthTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 3
    glUniform1i(depthMapID, 3);

    blaMat4 biasMatrix(
        0.5, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5, 0.0f,
        0.5, 0.5, 0.5, 1.0f
    );

    blaMat4 shadowMV = biasMatrix * shadowCamera->m_ViewProjection;
    GLuint shadowTID = glGetUniformLocation(prgmID, "shadowMV");
    glUniformMatrix4fv(shadowTID, 1, GL_FALSE, &shadowMV[0][0]);

    blaVec3 lightDirection = directionalLight->m_shadowRender.m_shadowDirection;

    GLuint lightID = glGetUniformLocation(prgmID, "lightDirection");
    glUniform3f(lightID, lightDirection.x, lightDirection.y, lightDirection.z);

    GLuint eyeID = glGetUniformLocation(prgmID, "eyePosition");
    blaScaledTransform camTransform = m_mainRenderCamera.m_attachedCamera->GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
    blaVec3 eye = camTransform.GetPosition();
    glUniform3f(eyeID, eye.x, eye.y, eye.z);

    GLuint shadowblaMapHandle = glGetUniformLocation(prgmID, "shadowMap");

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, directionalLight->m_shadowRender.m_depthTexture);

    glUniform1i(shadowblaMapHandle, 4);

    glBindVertexArray(m_screenSpaceQuad.m_vao);
    glEnableVertexAttribArray(0);

    // Draw the triangle !
    // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles


    glDisableVertexAttribArray(0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GL33Renderer::DrawPointLight(PointLightRender* pointLight)
{
    if (!m_pointLightSphereMesh.m_isInit)
        SetupPointLightRenderSphere();

    glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

    glDrawBuffer(GL_NONE);

    glDepthMask(GL_FALSE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
	
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glClear(GL_STENCIL_BUFFER_BIT);

    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    glUseProgram(m_GBuffer.m_drawSphereStencilPgrmID);

    blaMat4 modelMatrix = blaPosQuat(*pointLight->m_position, blaQuat()).ToMat4();

    blaMat4 MVP = m_mainRenderCamera.m_ViewProjection * modelMatrix;

    GLuint MVPid = glGetUniformLocation(m_GBuffer.m_drawSphereStencilPgrmID, "MVP");
    glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(m_pointLightSphereMesh.m_vao);

    // Draw VAO
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_pointLightSphereMesh.m_size), GL_UNSIGNED_INT, (void*)0); // Draw Triangles

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);

    // Multiple render targets code. This represent the output buffers of the fragment shading
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(1, DrawBuffers);

    glStencilFunc(GL_NOTEQUAL, 0, 0XFF);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    // Use our shader
    GLuint prgmID = m_glResources.m_glLoadedProgramsIds["PointLight"].m_loaded_id;
    glUseProgram(prgmID);

    MVPid = glGetUniformLocation(prgmID, "MVP");
    glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

    GLuint lightPrId = glGetUniformLocation(prgmID, "lightPR");
    blaVec4 lightPr = blaVec4(*pointLight->m_position, *pointLight->m_radius);
    glUniform4fv(lightPrId, 1, &(lightPr[0]));

    GLuint diffuseMapID = glGetUniformLocation(prgmID, "diffuseMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_diffuseTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(diffuseMapID, 0);

    GLuint normalMapID = glGetUniformLocation(prgmID, "normalMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_normalsTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 1
    glUniform1i(normalMapID, 1);

    GLuint worldPosMapID = glGetUniformLocation(prgmID, "worldPosMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_worldPosTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 2
    glUniform1i(worldPosMapID, 2);

    GLuint depthMapID = glGetUniformLocation(prgmID, "depthMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_depthTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 3
    glUniform1i(depthMapID, 3);

    glBindVertexArray(m_pointLightSphereMesh.m_vao);

    // Draw VAO
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_pointLightSphereMesh.m_size), GL_UNSIGNED_INT, (const void*)0); // Draw Triangles
    glStencilFunc(GL_NEVER, 0, 0);
    glCullFace(GL_BACK);
    glDisable(GL_STENCIL_TEST);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
    glUseProgram(0);
}

bool GL33Renderer::RenderDirectionalShadowMap(DirectionalShadowRender& shadowRender)
{
    shadowRender.Update();

    GLuint programId = m_glResources.m_glLoadedProgramsIds["ShadowMap"].m_loaded_id;

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowRender.m_shadowBuffer);

    // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glViewport(0, 0, shadowRender.m_bufferSize, shadowRender.m_bufferSize);

    // Clear Screen Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
        Render Scene Objects...
    */
    glUseProgram(programId);

    // Enable Z-Buffer test.
    glEnable(GL_DEPTH_TEST);
    for (size_t i = 0; i < m_meshRenderPool.size(); i++)
    {
        if (GL33RenderObject* renderObject = dynamic_cast<GL33RenderObject*>(m_meshRenderPool[i]))
        {
            blaMat4 MVP = shadowRender.getShadowViewProjection() * (*(renderObject->m_modelTransform));

            GLuint shadowMVID = glGetUniformLocation(programId, "depthMVP");
            glUniformMatrix4fv(shadowMVID, 1, GL_FALSE, &MVP[0][0]);

            glBindVertexArray(renderObject->m_vertexArrayID);

            // Draw VAO
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderObject->m_toMeshTriangles->size()), GL_UNSIGNED_INT, (void*)0);

            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void GL33Renderer::SetRenderSize(blaIVec2 renderSize)
{
    if (renderSize.x != m_renderSize.x || renderSize.y != m_renderSize.y)
    {
        ViewportResize(renderSize.x, renderSize.y);
    }
    Renderer::SetRenderSize(renderSize);
}

int BLA::GL33Renderer::SynchWithRenderManager()
{
    if (!m_renderingManager)
        return 0;

    blaVector<blaIndex> toErase;
    for (auto renderingObject : m_meshRenderPool)
    {
        if (m_renderingManager->GetTicketedMeshRenderers()->count((blaU32)renderingObject.first) == 0)
        {
            if (GL33RenderObject* gl33object = dynamic_cast<GL33RenderObject*>(renderingObject.second))
            {
                this->CleanUp(*gl33object);
                toErase.push_back(renderingObject.first);
            }
        }
    }

    for (auto ticket : toErase)
    {
        m_meshRenderPool.erase(ticket);
    }

    int addedObjectsOnCall = 0;

    for (auto ticketedObject : *(m_renderingManager->GetTicketedMeshRenderers()))
    {
        if (m_meshRenderPool.count(ticketedObject.first) == 0)
        {
            MeshRendererComponent* meshRenderer = ticketedObject.second;

            RenderObject* renderObject = this->LoadRenderObject(*meshRenderer, 0);

            m_meshRenderPool[ticketedObject.first] = renderObject;

            addedObjectsOnCall++;
        }
    }

    for (auto ticketedObject : *(m_renderingManager->GetTicketedDirectionalLights()))
    {
        if (m_directionalLightPool.count(ticketedObject.first) == 0)
        {
            blaPair<DirectionalLightComponent*, CameraComponent*> dirLightAndCamera = ticketedObject.second;

            DirectionalLightRender* dirLightRender = new DirectionalLightRender();
            dirLightRender->m_shadowRender.m_shadowCamera.AttachCamera(dirLightAndCamera.second);
            dirLightRender->m_shadowRender.m_shadowCamera.SetOrthographicProj(-200, 200, -200, 200);
            dirLightRender->m_shadowRender.m_bufferSize = 8192;
            SetupDirectionalShadowBuffer(dirLightRender->m_shadowRender);
            m_directionalLightPool[ticketedObject.first] = dirLightRender;

            addedObjectsOnCall++;
        }
    }

    for (auto ticketedObject : *(m_renderingManager->GetTicketedPointLights()))
    {
        if (m_directionalLightPool.count(ticketedObject.first) == 0)
        {
            PointLightComponent* pointLight = ticketedObject.second;

            PointLightRender* pointLightRender = new PointLightRender();
            pointLightRender->m_position = pointLight->GetPosition();
            pointLightRender->m_radius = pointLight->GetLightRadius();
            m_pointLightPool[ticketedObject.first] = pointLightRender;

            addedObjectsOnCall++;
        }
    }

    //if (type == 0)
    //    m_meshRenderPool.push_back(object);
    //else if (type == 1)
    //    m_gizmoRenderPool.push_back(object);
    return addedObjectsOnCall;
}

bool GL33Renderer::GenerateArrays(GL33RenderObject& object)
{
    //HARDCODED, suits:
    // Input vertex data, different for all executions of this shader.

    //layout(location = 0) in blaVec3 vertexPosition_modelspace;
    //layout(location = 1) in glm::vec2 vertexUV;
    //layout(location = 2) in blaVec3 vertexNormals;
    //layout(location = 3) in blaVec3 vertexTangent;
    //layout(location = 3) in blaVec4 vertexTangent;
    bool success = false;

    int layoutIndex = 0;

    GenerateBufferObject<blaVec3>(object, &((*object.m_toMeshVertices)[0]), static_cast<GLuint>(object.m_toMeshVertices->size() * sizeof(blaVec3)), 3, layoutIndex);
    layoutIndex++;
    if (!object.m_toMeshUVs->empty())
    {
        GenerateBufferObject<glm::vec2>(object, &((*object.m_toMeshUVs)[0]), static_cast<GLuint>(object.m_toMeshUVs->size() * sizeof(glm::vec2)), 2, layoutIndex);
        layoutIndex++;
    }

    if (!object.m_toMeshNormals->empty())
    {
        GenerateBufferObject<blaVec3>(object, &((*object.m_toMeshNormals)[0]), static_cast<GLuint>(object.m_toMeshNormals->size() * sizeof(blaVec3)), 3, layoutIndex);
        layoutIndex++;
    }

    if (!object.m_toMeshTangents->empty())
    {
        GenerateBufferObject<blaVec3>(object, &((*object.m_toMeshTangents)[0]), static_cast<GLuint>(object.m_toMeshTangents->size() * sizeof(blaVec3)), 3, layoutIndex);
        layoutIndex++;
    }

    if (!object.m_toMeshBiTangents->empty())
    {
        GenerateBufferObject<blaVec3>(object, &((*object.m_toMeshBiTangents)[0]), static_cast<GLuint>(object.m_toMeshBiTangents->size() * sizeof(blaVec3)), 3, layoutIndex);
        layoutIndex++;
    }

    GenerateElementBuffer(object, object.m_elementBufferId);

    success = true;

    return success;
}

template<typename objectType>
void GL33Renderer::GenerateBufferObject(GL33RenderObject& object, const objectType* buffer, GLuint bufferSize, GLuint elementsPerObject, GLuint attributeNumber)
{
    glBindVertexArray(object.m_vertexArrayID);

    // Load it into a VBO
    GLuint bufferObjectID;
    glGenBuffers(1, &bufferObjectID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObjectID);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, &(buffer[0]), GL_STATIC_DRAW);


    glEnableVertexAttribArray(attributeNumber);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObjectID);
    glVertexAttribPointer
    (
        attributeNumber,            // attribute #
        elementsPerObject,            // size
        GL_FLOAT,                    // type
        GL_FALSE,                    // normalized?
        0,                          // stride
        (void*)0                    // array buffer offset
    );

    glBindVertexArray(0);

    blaPair<GLuint, blaPair<GLuint, GLuint> > bufferObject = blaPair<GLuint, blaPair<GLuint, GLuint> >(bufferObjectID, blaPair<GLuint, GLuint>(attributeNumber, elementsPerObject));
    object.m_vboIDVector.push_back(bufferObject);
}

void GL33Renderer::GenerateElementBuffer(GL33RenderObject& object, GLuint elementBufferId)
{
    glBindVertexArray(object.m_vertexArrayID);
    glGenBuffers(1, &elementBufferId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.m_toMeshTriangles->size() * sizeof(GLuint), &(object.m_toMeshTriangles->at(0)), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void GL33Renderer::GenerateVertexArrayID(GL33RenderObject& object)
{
    // Vertex array of size 1 hardcoded for each MeshRendererComponent !
    glGenVertexArrays(1, &(object.m_vertexArrayID));
}

const Material* GL33Renderer::LoadMaterial(GL33RenderObject& object, blaString name)
{
    Asset* matAsset;
    AssetManager::AssetType matType = m_assetManager->GetAsset(name, matAsset);

    if (matAsset != nullptr && matType == AssetManager::AssetType::MaterialAsset)
    {
        if (Material* material = (Material*)matAsset)
        {
            for (auto texture : material->GetSamplers())
            {
                if (texture.second == "alphaMap") continue;
                Asset* texAsset;
                AssetManager::AssetType type = m_assetManager->GetAsset(texture.first, texAsset);
                if (type == AssetManager::AssetType::TextureAsset)
                {
                    Texture2D* textureAsset = (Texture2D*)texAsset;
                    m_glResources.GLLoadTexture(textureAsset->GetName(), *textureAsset);
                }
            }
            return material;
        }
    }
    else
    {
        Console::LogError("Could not find Material asset: " + name + "in resources.");
    }
    return nullptr;
}

bool GL33Renderer::CleanUp(GL33RenderObject& object)
{
    // Clean up all buffers associated to the mesh Vertex Array ID
    CleanUpVBOs(object);

    // Clean loaded textures
    for(const auto& m : object.m_materialAndIndex)
    {
	    if(m.first)
	    {
		    for(const auto& s : m.first->GetSamplers())
		    {
                const auto it = m_glResources.m_glLoadedTextureIds.find(s.first);
		    	if(it != m_glResources.m_glLoadedTextureIds.end())
		    	{
                    glDeleteTextures(1, &it->second);
                    m_glResources.m_glLoadedTextureIds.erase(it);
		    	}
		    }
	    }
    }

    //Cleanup Shader
    //glDeleteProgram(m_programID);
    DestroyVertexArrayID(object);
    return true;
}

void GL33Renderer::CleanUpVBOs(GL33RenderObject& object)
{
    // Cleanup VBO
    for (blaU32 vboIndex = 0; vboIndex < object.m_vboIDVector.size(); vboIndex++)
    {
        glDeleteBuffers(1, &((object.m_vboIDVector.at(vboIndex)).first));
    }
    object.m_vboIDVector.clear();
}

void GL33Renderer::DestroyVertexArrayID(GL33RenderObject& object)
{
    // Vertex array of size 1 hardcoded for each MeshRendererComponent !
    glDeleteVertexArrays(1, &object.m_vertexArrayID);
}

void GL33Renderer::CleanUpFrameDebug()
{
    if (m_debugLinesInfo.size != 0)
    {
        glDeleteBuffers(1, &(m_debugLinesInfo.vertBuffer));
        glDeleteBuffers(1, &(m_debugLinesInfo.colorBuffer));
        glDeleteVertexArrays(1, &(m_debugLinesInfo.vao));
    }

    if (m_debugMeshesInfo.size != 0)
    {
        glDeleteBuffers(1, &(m_debugMeshesInfo.vertBuffer));
        glDeleteBuffers(1, &(m_debugMeshesInfo.colorBuffer));
        glDeleteVertexArrays(1, &(m_debugMeshesInfo.vao));
    }
}

void GL33Renderer::InitializeRenderer(RenderWindow* window, RenderingManager* renderManager, DebugRenderingManager* debugRenderManager)
{
    m_renderingManager = renderManager;
    m_debugRenderingManager = debugRenderManager;

    bool initialized = false;

#ifdef GLFW_INTERFACE
    if (GLFWRenderWindow* render = dynamic_cast<GLFWRenderWindow*>(window))
    {
        // window-> IMPLEMENT CHECK WINDOW STATUS

        //Neat grey background
        glClearColor(0.f, 0.f, 0.f, 1.0f);

        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        // Cull triangles which normal is not towards the camera
        glEnable(GL_CULL_FACE);

        m_GBuffer.m_GbufferSize = m_renderSize;
        m_GBuffer.InitializeGBuffer();

        this->m_isContextEnabled = true;
        m_renderWindow = window;

        initialized = true;
    }

#elif defined(WPF_INTERFACE)
    if (WPFRenderWindow* render = dynamic_cast<WPFRenderWindow*>(window))
    {
        if (!initialized)
        {
            //Check for context validity:v

            // window-> IMPLEMENT CHECK WINDOW STATUS
            int x, y;
            render->GetSize(x, y);
            m_renderSize = glm::vec2(100, 100);

            //Neat grey background
            glClearColor(1.f, 0.f, 0.f, 1.0f);

            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);
            
            // Cull triangles which normal is not towards the camera
            glEnable(GL_CULL_FACE);

            m_GBuffer.m_GbufferSize = m_renderSize;
            m_GBuffer.InitializeGBuffer();

            this->m_isContextEnabled = true;
            m_renderWindow = window;

            initialized = true;
        }
    }
#endif

    if (!initialized)
    {
        std::cout << "No valid window handle provided to the Renderer\n";
        return;
    }

    // Hardcode system shaders loading
    this->m_glResources.m_systemShaders.LoadGeometryPassProgram("resources/shaders/GL33/Engine/GeomPassVS.glsl", "./resources/shaders/GL33/Engine/GeomPassFS.glsl");
    this->m_glResources.m_systemShaders.LoadDebugRaysProgram("./resources/shaders/GL33/Engine/DebugRaysShaderVS.glsl", "./resources/shaders/GL33/Engine/DebugRaysShaderFS.glsl");
    this->m_glResources.m_systemShaders.LoadDebugMeshesProgram("./resources/shaders/GL33/Engine/DebugMeshShaderVS.glsl", "./resources/shaders/GL33/Engine/DebugMeshShaderFS.glsl");
    this->m_glResources.m_systemShaders.LoadDepthBufferProgram("./resources/shaders/GL33/Engine/DrawDepthTextureVS.glsl", "./resources/shaders/GL33/Engine/DrawDepthTextureFS.glsl");
    this->m_glResources.m_systemShaders.LoadDrawColorBufferProgram("./resources/shaders/GL33/Engine/DrawColorTextureVS.glsl", "./resources/shaders/GL33/Engine/DrawColorTextureFS.glsl");
    this->m_glResources.m_systemShaders.LoadDrawNormalBufferProgram("./resources/shaders/GL33/Engine/DrawColorTextureVS.glsl", "./resources/shaders/GL33/Engine/DrawNormalTextureFS.glsl");
    this->m_glResources.m_systemShaders.LoadDrawSphereStencilProgram("./resources/shaders/GL33/Engine/PointLightStencilVS.glsl", "./resources/shaders/GL33/Engine/PointLightStencilFS.glsl");
    this->m_glResources.m_systemShaders.LoadShadowMapProgram("./resources/shaders/GL33/Engine/ShadowMapVS.glsl", "./resources/shaders/GL33/Engine/ShadowMapFS.glsl");

    this->m_glResources.GLLoadSystemShaders();

    m_GBuffer.m_drawSphereStencilPgrmID = this->m_glResources.m_systemShaders.m_drawSphereStencilPgrm.m_loaded_id;
    m_GBuffer.m_geometryPassPrgmID = this->m_glResources.m_systemShaders.m_geometryPassPrgm.m_loaded_id;
    m_drawColorBufferPrgmID = this->m_glResources.m_systemShaders.m_drawColorBufferPrgm.m_loaded_id;
    m_drawNormalsBufferPrgmID = this->m_glResources.m_systemShaders.m_drawNormalBufferPrgm.m_loaded_id;
    m_debugRayPgrmID = this->m_glResources.m_systemShaders.m_debugRayPgrm.m_loaded_id;
    m_debugMeshesPgrmID = this->m_glResources.m_systemShaders.m_debugMeshPgrm.m_loaded_id;

    GL33Shader dirLightShader = GL33Shader("DirectionalLight");
    dirLightShader.LoadShaderCode("./resources/shaders/GL33/Lighting/DirectLightVS.glsl", "./resources/shaders/GL33/Lighting/DirectLightFS.glsl");

    m_glResources.GLLoadShaderProgram(dirLightShader);

    GL33Shader pointLightShader = GL33Shader("PointLight");
    pointLightShader.LoadShaderCode("./resources/shaders/GL33/Lighting/PointLightVS.glsl", "./resources/shaders/GL33/Lighting/PointLightFS.glsl");

    m_glResources.GLLoadShaderProgram(pointLightShader);
}

void BLA::GL33Renderer::SwitchRenderingManager(RenderingManager * renderingManager)
{
    CleanUpPools();

    m_renderingManager = renderingManager;
}

bool GBuffer::InitializeGBuffer()
{
    // Create the FBO
    glGenFramebuffers(1, &m_frameBufferObject);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBufferObject);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Create the gbuffer textures
    glGenTextures(1, &m_diffuseTextureTarget);
    glGenTextures(1, &m_normalsTextureTarget);
    glGenTextures(1, &m_worldPosTextureTarget);
    glGenTextures(1, &m_texCoordsTextureTarget);
    glGenTextures(1, &m_depthTextureTarget);
    glGenTextures(1, &m_displayTextureTarget);

    // Bind Diffuse Texture
    glBindTexture(GL_TEXTURE_2D, m_diffuseTextureTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_GbufferSize.x, m_GbufferSize.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_diffuseTextureTarget, 0);

    // Bind Normals Texture
    glBindTexture(GL_TEXTURE_2D, m_normalsTextureTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_GbufferSize.x, m_GbufferSize.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalsTextureTarget, 0);

    // Bind WorldPos Texture
    glBindTexture(GL_TEXTURE_2D, m_worldPosTextureTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_GbufferSize.x, m_GbufferSize.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_worldPosTextureTarget, 0);

    // Bind TexCoords Texture
    glBindTexture(GL_TEXTURE_2D, m_texCoordsTextureTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_GbufferSize.x, m_GbufferSize.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_texCoordsTextureTarget, 0);

    // Bind Final Compositing Display Texture
    glBindTexture(GL_TEXTURE_2D, m_displayTextureTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_GbufferSize.x, m_GbufferSize.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_displayTextureTarget, 0);

    // depth
    glBindTexture(GL_TEXTURE_2D, m_depthTextureTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, m_GbufferSize.x, m_GbufferSize.y, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureTarget, 0);

    // Multiple render targets code. This represent the output buffers of the fragment shading
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        Console::LogMessage("FB error, status: 0x" + std::to_string(Status));
        return false;
    }

    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void GBuffer::SetupGeomPassMaterials(GLuint prgrmId)
{
    m_geometryPassPrgmID = prgrmId;
}

void GBuffer::DeleteGBufferResources()
{
    //Delete resources
	
	if(m_diffuseTextureTarget != 0xffffffff)
		glDeleteTextures(1, &m_diffuseTextureTarget);
	
    if (m_normalsTextureTarget != 0xffffffff)
		glDeleteTextures(1, &m_normalsTextureTarget);
	
    if (m_worldPosTextureTarget != 0xffffffff)
		glDeleteTextures(1, &m_worldPosTextureTarget);
	
    if (m_texCoordsTextureTarget != 0xffffffff)
		glDeleteTextures(1, &m_texCoordsTextureTarget);

    if (m_depthTextureTarget != 0xffffffff)
		glDeleteTextures(1, &m_depthTextureTarget);

    if (m_displayTextureTarget != 0xffffffff)
		glDeleteTextures(1, &m_displayTextureTarget);
	

    //Bind 0, which means render to back buffer, as a result, fb is unbound
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &m_frameBufferObject);
}

void GL33Renderer::SetupScreenSpaceRenderQuad()
{
    static const GLfloat g_quad_vertex_buffer_data[] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &(m_screenSpaceQuad.m_geomBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, m_screenSpaceQuad.m_geomBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glGenVertexArrays(1, &(m_screenSpaceQuad.m_vao));
    glBindVertexArray(m_screenSpaceQuad.m_vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenSpaceQuad.m_geomBuffer);
    glVertexAttribPointer(
        0,                  // Attribute #
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // ?normalized
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_screenSpaceQuad.m_isInit = true;
}

void GL33Renderer::SetupPointLightRenderSphere()
{
    m_pointLightSphereMesh.m_size = g_PointLightRenderSphere.m_renderData.m_triangleIndices.size();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &(m_pointLightSphereMesh.m_geomBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, m_pointLightSphereMesh.m_geomBuffer);
    glBufferData(GL_ARRAY_BUFFER, g_PointLightRenderSphere.m_renderData.m_vertPos.size() * sizeof(blaVec3), &(g_PointLightRenderSphere.m_renderData.m_vertPos[0]), GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glGenVertexArrays(1, &(m_pointLightSphereMesh.m_vao));
    glBindVertexArray(m_pointLightSphereMesh.m_vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointLightSphereMesh.m_geomBuffer);
    glVertexAttribPointer(
        0,                  // attribute#
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glGenBuffers(1, &(m_pointLightSphereMesh.m_elementBuffer));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pointLightSphereMesh.m_elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_PointLightRenderSphere.m_renderData.m_triangleIndices.size() * sizeof(GLuint), &(g_PointLightRenderSphere.m_renderData.m_triangleIndices[0]), GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_pointLightSphereMesh.m_isInit = true;
}

void GL33Renderer::RenderDebugLines()
{
    LoadDebugLines();

    glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

    glViewport(0, 0, m_renderSize.x, m_renderSize.y);

    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(m_debugLinesInfo.vao);

    glUseProgram(m_debugRayPgrmID);

    blaMat4 MVP = m_mainRenderCamera.m_ViewProjection; // model space is identity as it's loaded everything is loaded in worldspace

    GLuint MVPid = glGetUniformLocation(m_debugRayPgrmID, "MVP");
    glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

    GLuint worldPosMapID = glGetUniformLocation(m_debugRayPgrmID, "worldPosMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_worldPosTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(worldPosMapID, 0);

    GLuint displayBufferID = glGetUniformLocation(m_debugRayPgrmID, "displayBuffer");
    // Bind our texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_displayTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 1
    glUniform1i(displayBufferID, 1);

    glDrawArrays(0x0001, 0, static_cast<GLsizei>(m_debugLinesInfo.size));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GL33Renderer::RenderDebugMeshes()
{
    LoadDebugMeshes();

    glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

    glViewport(0, 0, m_renderSize.x, m_renderSize.y);

    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(m_debugMeshesInfo.vao);

    glUseProgram(m_debugMeshesPgrmID);

    blaMat4 MVP = m_mainRenderCamera.m_ViewProjection; // model space is identity as it's loaded everything is loaded in worldspace

    GLuint MVPid = glGetUniformLocation(m_debugMeshesPgrmID, "MVP");
    glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

    GLuint worldPosMapID = glGetUniformLocation(m_debugMeshesPgrmID, "worldPosMap");
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_worldPosTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(worldPosMapID, 0);

    GLuint displayBufferID = glGetUniformLocation(m_debugMeshesPgrmID, "displayBuffer");
    // Bind our texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_displayTextureTarget);
    // Set our "renderedTexture" sampler to user Texture Unit 1
    glUniform1i(displayBufferID, 1);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_debugMeshesInfo.size));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

void GL33Renderer::RenderDebug()
{
    if (m_renderDebug)
    {
        RenderDebugLines();
        RenderDebugMeshes();
    }
}

bool GL33Resources::GLLoadTexture(blaString resourcePath, const Texture2D& texture)
{
    if (m_glLoadedTextureIds.count(texture.GetName())) return true;
	
    glBindTexture(GL_TEXTURE_2D, 0);
	
    // Create one OpenGL texture
    GLuint texID = 0;
    glGenTextures(1, &texID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, texID);

    // Give the image to OpenGL
	if(texture.HasAlpha())
	{
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.m_width, texture.m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture.m_data.data());
	}
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.m_width, texture.m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture.m_data.data());
    }

    // Poor filtering, or ...
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // ... nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
	
    // Return the ID of the texture we just created
    m_glLoadedTextureIds[resourcePath] = texID;

    return true;
}

bool GL33Resources::GLLoadShaderProgram(GL33Shader& shader)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    Console::LogMessage("[SHADER]  Compiling shader: " + shader.m_name + ".");

    // Compile Vertex Shader
    //printf("[SHADER] Compiling shader : \"%s\"\n", vertexShader);
    char const * VertexSourcePointer = shader.m_vertexShader.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        blaVector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    char const * FragmentSourcePointer = shader.m_fragmentShader.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        blaVector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }


    // Link the program
    Console::LogMessage("[SHADER]  Linking Program: " + shader.m_name + ".");

    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        blaVector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    shader.m_loaded_id = ProgramID;

    m_glLoadedProgramsIds[shader.m_name] = shader;

    return true;
}

bool GL33Resources::GLLoadSystemShaders()
{
    this->GLLoadShaderProgram(this->m_systemShaders.m_drawColorBufferPrgm);
    this->GLLoadShaderProgram(this->m_systemShaders.m_drawNormalBufferPrgm);
    this->GLLoadShaderProgram(this->m_systemShaders.m_drawDepthBufferPrgm);
    this->GLLoadShaderProgram(this->m_systemShaders.m_geometryPassPrgm);
    this->GLLoadShaderProgram(this->m_systemShaders.m_drawSphereStencilPgrm);
    this->GLLoadShaderProgram(this->m_systemShaders.m_debugRayPgrm);
    this->GLLoadShaderProgram(this->m_systemShaders.m_debugMeshPgrm);
    this->GLLoadShaderProgram(this->m_systemShaders.m_shadowMapPgrm);

    return true;
}
