#include "WINGL33_Renderer.h"

GL33Renderer::GL33Renderer(char* windowTitle,bool isFullScreen):
	Renderer(windowTitle, isFullScreen)
{
	m_glfwWindow = InitializeContext(windowTitle);
	if(m_glfwWindow != NULL)
	{
		m_isContextEnabled = true;
	}
	WindowResize(m_glfwWindow, m_renderSize.x, m_renderSize.y);
}

GL33Renderer::~GL33Renderer() {}

void GL33Renderer::WindowResize(GLFWwindow* window, int width, int height)
{
	m_renderSize = ivec2(width, height);

	float aspectRatio = m_renderSize.x / m_renderSize.y;
	m_mainRenderCamera.SetPerspective(m_renderSize);

	m_GBuffer.DeleteGBufferResources();
	m_GBuffer.m_GbufferSize = m_renderSize;
	m_GBuffer.InitializeGBuffer();
}

vec2 GL33Renderer::GetCursorPosition()
{
	double x, y;
	glfwGetCursorPos(m_glfwWindow, &x, &y);
	return vec2(x, y);
}

GL33RenderObject::GL33RenderObject():
	m_vboIDVector(vector<pair<GLuint, pair<GLuint, GLuint> > >()),
	m_textureSamplersVector(vector<pair<GLuint, GLuint> >())
{}

GL33RenderObject::~GL33RenderObject() {}

void GL33Renderer::RenderGBuffer()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.m_frameBufferObject);

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
			GLuint transformID = glGetUniformLocation(renderObject->m_programID, "modelTransform");
			glUniformMatrix4fv(transformID, 1, GL_FALSE, &(renderObject->m_modelTransform->m_transformMatrix)[0][0]);

			// Send textureSamplers to shader
			for (int samplerIndex = 0; samplerIndex < renderObject->m_textureSamplersVector.size(); samplerIndex++)
			{
				glActiveTexture(GL_TEXTURE0 + samplerIndex);
				glBindTexture(GL_TEXTURE_2D, renderObject->m_textureSamplersVector.at(samplerIndex).second);

				glUniform1i(renderObject->m_textureSamplersVector.at(samplerIndex).first, samplerIndex);
			}

			glBindVertexArray(renderObject->m_vertexArrayID);

			// Draw VAO

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderObject->m_elementBufferId); // Index buffer
			glDrawElements(renderObject->m_renderType, renderObject->m_toMeshTriangles->size(), GL_UNSIGNED_INT, (void*)0); // Draw Triangles

			glBindVertexArray(0);

			for (uint vboIndex = 0; vboIndex < renderObject->m_vboIDVector.size(); vboIndex++)
			{
				glDisableVertexAttribArray(vboIndex);
			}
		}
	}
	glUseProgram(0);

}

bool GL33Renderer::Update()
{
	int width, height;
	glfwGetWindowSize(m_glfwWindow, &width, &height);
	
	if(width != m_renderSize.x || height != m_renderSize.y)
		WindowResize(m_glfwWindow, width, height);

	//Set OpenGL to this context.
	glfwMakeContextCurrent(m_glfwWindow);

	m_mainRenderCamera.Update();

	RenderGBuffer();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*DrawBufferOnScreen(ivec2(0, 0), ivec2(m_renderSize.x / 2, m_renderSize.y / 2), m_GBuffer.m_diffuseTextureTarget);
	DrawBufferOnScreen(ivec2(m_renderSize.x/2, 0), ivec2(m_renderSize.x, m_renderSize.y / 2), m_GBuffer.m_worldPosTextureTarget);
	DrawBufferOnScreen(ivec2(0, m_renderSize.y / 2), ivec2(m_renderSize.x / 2, m_renderSize.y), m_GBuffer.m_normalsTextureTarget);
	DrawBufferOnScreen(ivec2(m_renderSize.x / 2, m_renderSize.y / 2), ivec2(m_renderSize.x, m_renderSize.y), m_GBuffer.m_texCoordsTextureTarget);
	*/

	for (DirectionalLightRender directLight : m_directionalLightsVector)
	{
		RenderDirectionalShadowMap(directLight.m_shadowRender);
		DrawDirectionalLight(directLight);
	}

	glfwSwapInterval(0);
	
	glfwSwapBuffers(m_glfwWindow);
	glfwPollEvents();
	
	return true;
}

RenderObject* GL33Renderer::LoadRenderObject(const MeshRenderer& meshRenderer, int type)
{
	GL33RenderObject* object = new GL33RenderObject();
	this->GenerateVertexArrayID(*object);

	object->m_toMeshTriangles = &(meshRenderer.m_meshTriangles);
	object->m_toMeshVertices = &(meshRenderer.m_meshVertices);
	object->m_toMeshNormals = &(meshRenderer.m_meshNormals);
	object->m_toMeshTangents = &(meshRenderer.m_meshTangents);
	object->m_toMeshBiTangents = &(meshRenderer.m_meshBiTangents);
	object->m_toMeshUVs = &(meshRenderer.m_meshUVs);

	object->m_modelTransform = meshRenderer.m_modelTransform;

	if (!this->GenerateArrays(*object))
	{
		return NULL;
	}

	for (int i = 0; i < meshRenderer.m_materials.size(); i++)
	{
		AssignMaterial(*object, meshRenderer.m_materials[i]);
	}

	for (int i = 0; i < meshRenderer.m_textures.size(); i++)
	{
		LoadTextureSample(*object, meshRenderer.m_textures[i].first, meshRenderer.m_textures[i].second);
	}

	object->m_renderType = meshRenderer.m_renderType;

	if (type == 0)
		m_renderPool.push_back(object);
	else if (type == 1)
		m_gizmoRenderPool.push_back(object);
	else if (type == 2)
		debug_sphere = object;

	return object;
}

bool GL33Renderer::CancelRender(const MeshRenderer& object)
{
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowRender.m_bufferSize, shadowRender.m_bufferSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

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
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
	glDisable(GL_COMPARE_R_TO_TEXTURE);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

void GL33Renderer::DrawDepthBufferOnScreen(ivec2 topLeft, ivec2 bottomRight, GLuint textureTarget)
{
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glViewport(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

	// Use our shader
	glUseProgram(DrawDepthBufferPrgmID);
	GLuint texID = glGetUniformLocation(DrawDepthBufferPrgmID, "depthTexture");
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTarget);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(texID, 0);

	// 1rst attribute buffer : vertices
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
	glDisable(GL_COMPARE_R_TO_TEXTURE);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

void GL33Renderer::DrawDirectionalLight(DirectionalLightRender directionalLight)
{
	OrthographicCamera* shadowCamera = &(directionalLight.m_shadowRender.m_shadowCamera);
	// Memory leak here <-- !!
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

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
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(normalMapID, 1);

	GLuint worldPosMapID = glGetUniformLocation(prgmID, "worldPosMap");
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_GBuffer.m_worldPosTextureTarget);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(worldPosMapID, 2);

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	mat4 shadowMV = biasMatrix * shadowCamera->m_ViewProjection;
	GLuint shadowTID = glGetUniformLocation(prgmID, "shadowMV");
	glUniformMatrix4fv(shadowTID, 1, GL_FALSE, &shadowMV[0][0]);

	vec3 lightDirection = directionalLight.m_lightDirection;

	GLuint lightID = glGetUniformLocation(prgmID, "lightDirection");
	glUniform3f(lightID, lightDirection.x, lightDirection.y, lightDirection.z);

	GLuint shadowmapHandle = glGetUniformLocation(prgmID, "shadowMap");

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, directionalLight.m_shadowRender.m_depthTexture);

	glUniform1i(shadowmapHandle, 3);

	// 1rst attribute buffer : vertices
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// Draw the triangle !
	// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
	glDisable(GL_COMPARE_R_TO_TEXTURE);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

bool GL33Renderer::RenderDirectionalShadowMap(DirectionalShadowRender& shadowRender)
{
	shadowRender.Update();

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowRender.m_shadowBuffer);
	glViewport(0, 0, shadowRender.m_bufferSize, shadowRender.m_bufferSize); // Render on the whole framebuffer, complete from the lower left corner to the upper righ

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
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderObject->m_elementBufferId); // Index buffer
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
	//layout(location = 4) in vec3 vertexBiTangent;
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
		GenerateBufferObject<vec3>(object, &((*object.m_toMeshNormals)[0]), object.m_toMeshTangents->size() * sizeof(vec3), 3, layoutIndex);
		layoutIndex++;
	}

	if (!object.m_toMeshBiTangents->empty())
	{
		GenerateBufferObject<vec3>(object, &((*object.m_toMeshNormals)[0]), object.m_toMeshBiTangents->size() * sizeof(vec3), 3, layoutIndex);
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
		attributeNumber,			// attribute
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
	extern SharedResources* sharedResources;
	object.m_programID = sharedResources->GetMaterial(name.data());
	if (object.m_programID != 0)
	{
		object.m_matrixID = glGetUniformLocation(object.m_programID, "MVP");
		return true;
	}
	return false;
}

bool GL33Renderer::LoadTextureSample(GL33RenderObject& object, string textureName, string sampleName)
{
	extern SharedResources* sharedResources;
	if (object.m_programID != 0)
	{
		GLuint textureID = glGetUniformLocation(object.m_programID, sampleName.data());
		GLuint textureRessource = sharedResources->GetTexture(textureName.data());
		object.m_textureSamplersVector.push_back(pair<GLuint, GLuint>(textureID, textureRessource));

		return true;
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

GLFWwindow* GL33Renderer::InitializeContext(char* windowTitle)
{
	
	GLFWwindow* window;
	// Initialise GLFW
	if( !glfwInit() )
	{
		printf("Failed to initialize GLFW\n");
		//Debug::OutputToDebug("Failed to initialize GLFW\n" );
		return NULL;
	}
	
	//glfwWindowHint(GLFW_SAMPLES , 16 );
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR , 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR , 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	
	// Open a window and create its OpenGL context
	
	GLFWmonitor* monitor = NULL;

	if(m_isFullScreen)
	{
		monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		m_renderSize = ivec2(videoMode->width,videoMode->height);
	}
	
	window = glfwCreateWindow(m_renderSize.x, m_renderSize.y, windowTitle, monitor,NULL);
	
	glfwMakeContextCurrent(window);
	
	// Initialize GLEW
	
	glewExperimental = true; // Needed for core profile
	
	
	if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW\n");
		//Debug::OutputToDebug("Failed to initialize GLEW\n");
		return NULL;
	}
	
	glfwSetWindowTitle( window,windowTitle );
	
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	glfwSetCursorPos(window, m_renderSize.x/2, m_renderSize.y/2);
	
	//Neat grey background
	glClearColor(0.f, 0.f, 0.f, 0.0f);
	
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	m_GBuffer.m_GbufferSize = m_renderSize;
	m_GBuffer.InitializeGBuffer();
		
	return window;
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

	// depth
	glBindTexture(GL_TEXTURE_2D, m_depthTextureTarget);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_GbufferSize.x, m_GbufferSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureTarget, 0);

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

	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &m_frameBufferObject);
}
