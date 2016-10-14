#include "GL33Renderer.h"
using namespace BLAengine;

GL33Renderer::GL33Renderer():
	debug_renderGBuffer(false),
	m_renderDebug(true)
{}

GL33Renderer::~GL33Renderer() {}

void GL33Renderer::ViewportResize(int width, int height)
{
	m_renderSize = ivec2(width, height);

	float aspectRatio = m_renderSize.x / m_renderSize.y;
	m_mainRenderCamera.SetPerspective(m_renderSize);

	m_GBuffer.DeleteGBufferResources();
	m_GBuffer.m_GbufferSize = m_renderSize;
	m_GBuffer.InitializeGBuffer();
}

GL33RenderObject::GL33RenderObject():
	m_vboIDVector(vector<pair<GLuint, pair<GLuint, GLuint> > >()),
	m_textureSamplersVector(vector<pair<GLuint, GLuint> >())
{}

GL33RenderObject::~GL33RenderObject() {}
 
/*
	Rendering code below
*/

bool GL33Renderer::Update()
{
	int width, height;
	m_renderWindow->GetSize(width, height);

	if (width != m_renderSize.x || height != m_renderSize.y)
	{
		ViewportResize(width, height);
	}
		
	//Set OpenGL to this context.
	m_renderWindow->MakeGLContextCurrent();
	
	m_mainRenderCamera.Update();

	RenderGBuffer();

	// Draw vignettes with buffer results (from Gbuffer pass)
	if (debug_renderGBuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		DrawColorBufferOnScreen(ivec2(0, 0), ivec2(m_renderSize.x / 2, m_renderSize.y / 2), m_GBuffer.m_diffuseTextureTarget);
		DrawColorBufferOnScreen(ivec2(m_renderSize.x / 2, 0), ivec2(m_renderSize.x, m_renderSize.y / 2), m_GBuffer.m_worldPosTextureTarget);
		DrawColorBufferOnScreen(ivec2(0, m_renderSize.y / 2), ivec2(m_renderSize.x / 2, m_renderSize.y), m_GBuffer.m_normalsTextureTarget);
		DrawColorBufferOnScreen(ivec2(m_renderSize.x / 2, m_renderSize.y / 2), ivec2(m_renderSize.x, m_renderSize.y), m_GBuffer.m_texCoordsTextureTarget);
	}
	else
	{

		glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(1, DrawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		for (PointLightRender pointLight : m_pointLightsVector)
		{
			DrawPointLight(pointLight);
		}


		for (DirectionalLightRender directLight : m_directionalLightsVector)
		{
			RenderDirectionalShadowMap(directLight.m_shadowRender);
			DrawDirectionalLight(directLight);
		}

		DrawDisplayBuffer();
		RenderDebug();

	}

	CleanUpFrameDebug();

	m_renderWindow->UpdateWindowAndBuffers();

	return true;
}

void GL33Renderer::RenderGBuffer()
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

	// Multiple render targets code. This represent the output buffers of the fragment shading
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, DrawBuffers);

	glViewport(0, 0, m_GBuffer.m_GbufferSize.x, m_GBuffer.m_GbufferSize.y);

	// Clear Frame Buffer.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_GBuffer.m_geometryPassPrgmID);

	for (int i = 0; i < m_renderPool.size(); i++)
	{
		if (GL33RenderObject* renderObject = dynamic_cast<GL33RenderObject*>(m_renderPool[i]))
		{
			mat4 MVP = m_mainRenderCamera.m_ViewProjection  * (renderObject->m_modelTransform->m_transformMatrix);

			GLuint MVPid = glGetUniformLocation(m_GBuffer.m_geometryPassPrgmID, "MVP");
			glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

			//send modelTransform to shader
			GLuint transformID = glGetUniformLocation(m_GBuffer.m_geometryPassPrgmID, "modelTransform");
			glUniformMatrix4fv(transformID, 1, GL_FALSE, &(renderObject->m_modelTransform->m_transformMatrix)[0][0]);

			// Send textureSamplers to shader
			for (uint16 samplerIndex = 0; samplerIndex < renderObject->m_textureSamplersVector.size(); samplerIndex++)
			{
				glActiveTexture(GL_TEXTURE0 + samplerIndex);

				glBindTexture(GL_TEXTURE_2D, renderObject->m_textureSamplersVector.at(samplerIndex).second);

				glUniform1i(renderObject->m_textureSamplersVector.at(samplerIndex).first, samplerIndex);
			}

			glBindVertexArray(renderObject->m_vertexArrayID);

			// Draw VAO
			glDrawElements(renderObject->m_renderType, renderObject->m_toMeshTriangles->size(), GL_UNSIGNED_INT, (void*)0); // Draw Triangles

			glBindVertexArray(0);
			// Send textureSamplers to shader
			for (uint16 samplerIndex = 0; samplerIndex < renderObject->m_textureSamplersVector.size(); samplerIndex++)
			{
				glActiveTexture(GL_TEXTURE0 + samplerIndex);
			}

			for (uint vboIndex = 0; vboIndex < renderObject->m_vboIDVector.size(); vboIndex++)
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

	DrawColorBufferOnScreen(ivec2(0,0), m_renderSize, m_GBuffer.m_displayTextureTarget);
}

RenderObject* GL33Renderer::LoadRenderObject(const MeshRenderer& meshRenderer, int type)
{
	GL33RenderObject* object = new GL33RenderObject();
	this->GenerateVertexArrayID(*object);
	RenderData* renderData = &(meshRenderer.m_mesh->m_renderData);
	object->m_toMeshTriangles = &(renderData->m_triangleIndices);
	object->m_toMeshVertices = &(renderData->m_vertPos);
	object->m_toMeshNormals = &(renderData->m_vertNormal);
	object->m_toMeshTangents = &(renderData->m_vertTangent);
	object->m_toMeshBiTangents = &(renderData->m_vertBiTangent);
	object->m_toMeshUVs = &(renderData->m_vertUVs);

	object->m_modelTransform = meshRenderer.m_modelTransform;

	if (!this->GenerateArrays(*object))
	{
		return nullptr;
	}

	for (int i = 0; i < meshRenderer.m_materials.size(); i++)
	{
		Material* mat = meshRenderer.m_materials[i];

		AssignMaterial(*object, mat->GetName());

		for (int j = 0; j < mat->m_textureSamplerAttributes.size(); j++)
		{
			LoadTextureSample(*object, mat->m_textureSamplerAttributes[j].first, mat->m_textureSamplerAttributes[j].second);
		}
	}

	object->m_renderType = meshRenderer.m_renderType;

	if (type == 0)
		m_renderPool.push_back(object);
	else if (type == 1)
		m_gizmoRenderPool.push_back(object);

	return object;
}

bool GL33Renderer::CancelRender(const MeshRenderer& object)
{
	return true;
}

bool GL33Renderer::LoadDebugLines(pair<vector<vec3>, vector<vec3>>& debugLinesMesh)
{
	if (debugLinesMesh.first.size() == 0 || debugLinesMesh.second.size() != debugLinesMesh.first.size())
	{
		m_debugLinesInfo = { 0, 0, 0, 0 };
		return false;
	}

	m_debugLinesInfo.size = debugLinesMesh.first.size();

	glGenBuffers(1, &(m_debugLinesInfo.vertBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, m_debugLinesInfo.vertBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_debugLinesInfo.size * sizeof(vec3), &(debugLinesMesh.first[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &(m_debugLinesInfo.colorBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, m_debugLinesInfo.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_debugLinesInfo.size * sizeof(vec3), &(debugLinesMesh.second[0]), GL_STATIC_DRAW);

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

	return true;
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

	return true;
}

void GL33Renderer::DrawColorBufferOnScreen(ivec2 topLeft, ivec2 bottomRight, GLuint textureTarget)
{
	if (!m_screenSpaceQuad.m_isInit)
		SetupScreenSpaceRenderQuad();

	glDisable(GL_BLEND);

	//glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	glViewport(topLeft.x, topLeft.y, bottomRight.x-topLeft.x, bottomRight.y-topLeft.y);

	// Use our shader
	glUseProgram(DrawColorBufferPrgmID);
	GLuint texID = glGetUniformLocation(DrawColorBufferPrgmID, "colorTexture");
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
}

void GL33Renderer::DrawDepthBufferOnScreen(ivec2 topLeft, ivec2 bottomRight, GLuint textureTarget)
{
	if (!m_screenSpaceQuad.m_isInit)
		SetupScreenSpaceRenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

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
}

void GL33Renderer::DrawDirectionalLight(DirectionalLightRender directionalLight)
{
	if (!m_screenSpaceQuad.m_isInit)
		SetupScreenSpaceRenderQuad();

	OrthographicCamera* shadowCamera = &(directionalLight.m_shadowRender.m_shadowCamera);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

	glDisable(GL_DEPTH_TEST);

	// Multiple render targets code. This represent the output buffers of the fragment shading
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(1, DrawBuffers);

	//glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, m_renderSize.x, m_renderSize.y);

	// Use our shader
	GLuint prgmID = directionalLight.m_lightRenderPrgmID;
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

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	mat4 shadowMV = biasMatrix * shadowCamera->m_ViewProjection;
	GLuint shadowTID = glGetUniformLocation(prgmID, "shadowMV");
	glUniformMatrix4fv(shadowTID, 1, GL_FALSE, &shadowMV[0][0]);

	vec3 lightDirection = directionalLight.m_shadowRender.m_shadowDirection;

	GLuint lightID = glGetUniformLocation(prgmID, "lightDirection");
	glUniform3f(lightID, lightDirection.x, lightDirection.y, lightDirection.z);

	GLuint eyeID = glGetUniformLocation(prgmID, "eyePosition");
	vec3 eye = m_mainRenderCamera.m_attachedCamera->m_transform->m_position;
	glUniform3f(eyeID, eye.x, eye.y, eye.z);

	GLuint shadowmapHandle = glGetUniformLocation(prgmID, "shadowMap");

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, directionalLight.m_shadowRender.m_depthTexture);

	glUniform1i(shadowmapHandle, 4);

	glBindVertexArray(m_screenSpaceQuad.m_vao);
	glEnableVertexAttribArray(0);

	// Draw the triangle !
	// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	
	
	glDisableVertexAttribArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GL33Renderer::DrawPointLight(PointLightRender pointLight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

	glDrawBuffer(GL_NONE);

	glDepthMask(GL_FALSE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	
	glClear(GL_STENCIL_BUFFER_BIT);

	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	glUseProgram(m_GBuffer.m_drawSphereStencilPgrmID);

	mat4 MVP = m_mainRenderCamera.m_ViewProjection * pointLight.m_transform->m_transformMatrix;

	GLuint MVPid = glGetUniformLocation(m_GBuffer.m_drawSphereStencilPgrmID, "MVP");
	glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

	glBindVertexArray(m_pointLightSphereMesh.m_vao);

	// Draw VAO
	glDrawElements(GL_TRIANGLES, m_pointLightSphereMesh.m_size, GL_UNSIGNED_INT, (void*) 0); // Draw Triangles

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

	glUseProgram(pointLight.m_pointLightPrgmID);

	MVPid = glGetUniformLocation(pointLight.m_pointLightPrgmID, "MVP");
	glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

	GLuint lightPrId = glGetUniformLocation(pointLight.m_pointLightPrgmID, "lightPR");
	vec4 lightPr = vec4(pointLight.m_transform->m_position, length(pointLight.m_transform->m_scale)/2);
	glUniform4fv(lightPrId, 1, &(lightPr[0]));

	// Use our shader
	GLuint prgmID = pointLight.m_pointLightPrgmID;
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

	glBindVertexArray(m_pointLightSphereMesh.m_vao);

	// Draw VAO
	glDrawElements(GL_TRIANGLES, m_pointLightSphereMesh.m_size, GL_UNSIGNED_INT, (void*)0); // Draw Triangles
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

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowRender.m_shadowBuffer);

	// Render on the whole framebuffer, complete from the lower left corner to the upper right
	glViewport(0, 0, shadowRender.m_bufferSize, shadowRender.m_bufferSize); 

	// Clear Screen Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
		Render Scene Objects...
	*/
	glUseProgram(shadowRender.m_shadowPrgmID);

	// Enable Z-Buffer test.
	glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < m_renderPool.size(); i++)
	{
		if (GL33RenderObject* renderObject = dynamic_cast<GL33RenderObject*>(m_renderPool[i]))
		{
			mat4 MVP = shadowRender.getShadowViewProjection() * (renderObject->m_modelTransform->m_transformMatrix);

			GLuint shadowMVID = glGetUniformLocation(shadowRender.m_shadowPrgmID, "depthMVP");
			glUniformMatrix4fv(shadowMVID, 1, GL_FALSE, &MVP[0][0]);

			glBindVertexArray(renderObject->m_vertexArrayID);

			// Draw VAO
			glDrawElements(GL_TRIANGLES, renderObject->m_toMeshTriangles->size(), GL_UNSIGNED_INT, (void*)0);

			glBindVertexArray(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

bool GL33Renderer::GenerateArrays(GL33RenderObject& object)
{
	//HARDCODED, suits:
	// Input vertex data, different for all executions of this shader.

	//layout(location = 0) in vec3 vertexPosition_modelspace;
	//layout(location = 1) in vec2 vertexUV;
	//layout(location = 2) in vec3 vertexNormals;
	//layout(location = 3) in vec3 vertexTangent;
	bool success = false;

	int layoutIndex = 0;

	GenerateBufferObject<vec3>(object, &((*object.m_toMeshVertices)[0]), object.m_toMeshVertices->size() * sizeof(vec3), 3, layoutIndex);
	layoutIndex++;
	if (!object.m_toMeshUVs->empty())
	{
		GenerateBufferObject<vec2>(object, &((*object.m_toMeshUVs)[0]), object.m_toMeshUVs->size() * sizeof(vec2), 2, layoutIndex);
		layoutIndex++;
	}

	if (!object.m_toMeshNormals->empty())
	{
		GenerateBufferObject<vec3>(object, &((*object.m_toMeshNormals)[0]), object.m_toMeshNormals->size() * sizeof(vec3), 3, layoutIndex);
		layoutIndex++;
	}

	if (!object.m_toMeshTangents->empty())
	{
		GenerateBufferObject<vec3>(object, &((*object.m_toMeshTangents)[0]), object.m_toMeshTangents->size() * sizeof(vec3), 3, layoutIndex);
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
		attributeNumber,			// attribute #
		elementsPerObject,			// size
		GL_FLOAT,					// type
		GL_FALSE,					// normalized?
		0,                          // stride
		(void*)0					// array buffer offset
		);

	glBindVertexArray(0);

	pair<GLuint, pair<GLuint, GLuint> > bufferObject = pair<GLuint, pair<GLuint, GLuint> >(bufferObjectID, pair<GLuint, GLuint>(attributeNumber, elementsPerObject));
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
	// Vertex array of size 1 hardcoded for each MeshRenderer !
	glGenVertexArrays(1, &(object.m_vertexArrayID));
}

bool GL33Renderer::AssignMaterial(GL33RenderObject& object, string name)
{
	if (m_glResources.m_glLoadedProgramsIds.count(name))
	{
		object.m_programID = m_glResources.m_glLoadedProgramsIds[name];
		if (object.m_programID != 0)
		{
			object.m_matrixID = glGetUniformLocation(object.m_programID, "MVP");
			return true;
		}
	}
	else
	{
		AssetManager::AssetType type;
		Asset* matAsset = m_assetManager->GetAsset(name, type);

		if (matAsset != nullptr && type == AssetManager::AssetType::MaterialAsset)
		{
			if (Material* material = (Material*) matAsset)
			{
				m_glResources.GLLoadShaderProgram(name, material->m_fragmentShader, material->m_vertexShader);

				cout << "Material " << name << " loaded into GL \n";

				return this->AssignMaterial(object, name);
			}
		}

		cout << "Could not find shader loaded shader pgrm: " << name << "\n";
	}
	return false;
}

bool GL33Renderer::LoadTextureSample(GL33RenderObject& object, string textureName, string sampleName)
{
	if (object.m_programID != 0)
	{
		if (m_glResources.m_glLoadedTextureIds.count(textureName))
		{
			GLuint glResourceTextureId = m_glResources.m_glLoadedTextureIds[textureName];
			GLuint textureHandleID = glGetUniformLocation(this->m_GBuffer.m_geometryPassPrgmID, sampleName.data());

			object.m_textureSamplersVector.push_back(pair<GLuint, GLuint>(textureHandleID, glResourceTextureId));

			return true;
		}
	}
	return false;
}

bool GL33Renderer::CleanUp(GL33RenderObject& object)
{
	// Clean up all buffers associated to the mesh Vertex Array ID
	CleanUpVBOs(object);

	// Clean texture samplers 
	for (GLushort texIndex = 0; texIndex < object.m_textureSamplersVector.size(); texIndex++)
	{
		glDeleteTextures(1, &(object.m_textureSamplersVector.at(texIndex).first));
	}
	object.m_textureSamplersVector.clear();

	//Cleanup Shader
	//glDeleteProgram(m_programID);
	DestroyVertexArrayID(object);
	return true;
}

void GL33Renderer::CleanUpVBOs(GL33RenderObject& object)
{
	// Cleanup VBO
	for (uint vboIndex = 0; vboIndex < object.m_vboIDVector.size(); vboIndex++)
	{
		glDeleteBuffers(1, &((object.m_vboIDVector.at(vboIndex)).first));
	}
	object.m_vboIDVector.clear();
}

void GL33Renderer::DestroyVertexArrayID(GL33RenderObject& object)
{
	// Vertex array of size 1 hardcoded for each MeshRenderer !
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
}

void GL33Renderer::InitializeRenderer(RenderWindow* window)
{
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
#endif
	if (WPFRenderWindow* render = dynamic_cast<WPFRenderWindow*>(window))
	{
		if (!initialized)
		{
			//Check for context validity:

			// window-> IMPLEMENT CHECK WINDOW STATUS
			int x, y;
			render->GetSize(x, y);
			m_renderSize = ivec2(100, 100);

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
	if(!initialized)
	{
		std::cout << "No valid window handle provided to the Renderer\n";
	}
}

bool GBuffer::InitializeGBuffer()
{
	// Create the FBO
	glGenFramebuffers(1, &m_frameBufferObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBufferObject);

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

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::SetupGeomPassMaterials(GLuint prgrmId)
{
	m_geometryPassPrgmID = prgrmId;
}

void GBuffer::DeleteGBufferResources()
{
	//Delete resources
	glDeleteTextures(1, &m_diffuseTextureTarget);
	glDeleteTextures(1, &m_normalsTextureTarget);
	glDeleteTextures(1, &m_worldPosTextureTarget);
	glDeleteTextures(1, &m_texCoordsTextureTarget);
	glDeleteTextures(1, &m_depthTextureTarget);
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

void GL33Renderer::SetupPointLightRenderSphere(vector<vec3> sphereMeshVertices, vector<GLuint> indices)
{
	if (sphereMeshVertices.empty())
	{
		return;
	}

	m_pointLightSphereMesh.m_size = indices.size();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &(m_pointLightSphereMesh.m_geomBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, m_pointLightSphereMesh.m_geomBuffer);
	glBufferData(GL_ARRAY_BUFFER, sphereMeshVertices.size() * sizeof(vec3), &(sphereMeshVertices[0]), GL_STATIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &(indices[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_pointLightSphereMesh.m_isInit = true;
}

void GL33Renderer::RenderDebugLines()
{
	if (m_debugLinesInfo.size == 0)
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, m_renderSize.x, m_renderSize.y);
	
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(m_debugLinesInfo.vao);

	glUseProgram(m_debugRayPgrmID);

	mat4 MVP = m_mainRenderCamera.m_ViewProjection;

	GLuint MVPid = glGetUniformLocation(m_debugRayPgrmID, "MVP");
	glUniformMatrix4fv(MVPid, 1, GL_FALSE, &MVP[0][0]);

	GLuint depthMapID = glGetUniformLocation(m_debugRayPgrmID, "depthMap");
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_depthTextureTarget);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(depthMapID, 0);

	GLuint displayBufferID = glGetUniformLocation(m_debugRayPgrmID, "displayBuffer");
	// Bind our texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_displayTextureTarget);
	// Set our "renderedTexture" sampler to user Texture Unit 1
	glUniform1i(displayBufferID, 1);

	glDrawArrays(0x0001, 0, m_debugLinesInfo.size);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

bool BLAengine::GL33Resources::GLLoadTexture(std::string resourcePath, Texture2D texture)
{
	// Create one OpenGL texture
	GLuint texID;
	glGenTextures(1, &texID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, texID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.m_width, texture.m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture.m_data.data());

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	m_glLoadedTextureIds[resourcePath] = texID;

	return true;
}

bool BLAengine::GL33Resources::GLLoadShaderProgram(std::string name, std::string fragmentShader, std::string vertexShader)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexShader, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("%s could not be opened.\n", vertexShader);
		return false;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentShader, std::ios::in);
	if (FragmentShaderStream.is_open())
	{

		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	printf("[SHADER]: \"%s\"\n", name);

	// Compile Vertex Shader
	printf("[SHADER] Compiling shader : \"%s\"\n", vertexShader);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("[SHADER] Compiling shader : %s\n", fragmentShader);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	// Link the program
	printf("[SHADER] Linking program \"%s\"\n", name);
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	m_glLoadedProgramsIds[name] = ProgramID;

	return true;
}