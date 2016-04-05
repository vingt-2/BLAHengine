#include "WINGL33_Renderer.h"

GL33Renderer::GL33Renderer(char* windowTitle,bool isFullScreen):
	Renderer(windowTitle, isFullScreen)
{
	glfwWindow = InitializeContext(windowTitle);
	if(glfwWindow != NULL)
	{
		isContextEnabled = true;
	}
}

GL33Renderer::~GL33Renderer()
{
}

GL33RenderObject::GL33RenderObject():
	m_vboIDVector(vector<pair<GLuint, pair<GLuint, GLuint> > >()),
	m_textureSamplersVector(vector<pair<GLuint, GLuint> >())
{}

GL33RenderObject::~GL33RenderObject()
{
}

bool GL33Renderer::Update()
{
	//Set OpenGL to this context.
	glfwMakeContextCurrent(glfwWindow);
	
	//Adjust Projection and ViewPort.
	m_mainCamera->SetProjection(glm::frustum(-0.0001f*renderSize.x, 0.0001f*renderSize.x, -0.0001f*renderSize.y, 0.0001f*renderSize.y, 0.1f, 100.0f));
	glViewport(0,0,renderSize.x,renderSize.y);
	
	// Clear Screen Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Render Scene Objects.
	
	// Enable Z-Buffer test.
	glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < m_renderPool.size(); i++)
	{
		if (GL33RenderObject* renderObject = dynamic_cast<GL33RenderObject*>(m_renderPool[i]))
		{
			this->Draw(*renderObject);
		}
	}

	// Render Debug gizmos (on top of frame)
	// Disable Z-Buffer test so that it draws on top of scene.
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < m_gizmoRenderPool.size(); i++)
	{
		if (GL33RenderObject* renderObject = dynamic_cast<GL33RenderObject*>(m_gizmoRenderPool[i]))
		{
			this->Draw(*renderObject);
		}
	}
	for (auto p : debug_sphere_requests)
	{
		//cout << "rendering sphere";
		if (GL33RenderObject* renderObject = dynamic_cast<GL33RenderObject*>(debug_sphere))
		{
			renderObject->m_modelTransform->position = p;
			renderObject->m_modelTransform->UpdateTransform();
			this->Draw(*renderObject);
		}
	}
	debug_sphere_requests.clear();
	for (int i = 0; i < m_gizmoRenderPool.size(); i++)
	{
		if (GL33RenderObject* renderObject = dynamic_cast<GL33RenderObject*>(m_gizmoRenderPool[i]))
		{
			CleanUp(*renderObject);
		}
	}
	m_gizmoRenderPool.clear();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDisable(GL_BLEND);
	
	glfwSwapInterval(1);
	
	glfwSwapBuffers(glfwWindow);
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

bool GL33Renderer::Draw(GL33RenderObject& object)
{
	glUseProgram(object.m_programID);

	
	mat4 MVP = this->m_mainCamera->m_projection * this->m_mainCamera->m_viewTransform.transformMatrix * (object.m_modelTransform->transformMatrix);
	glUniformMatrix4fv(object.m_matrixID, 1, GL_FALSE, &MVP[0][0]);

	//send modelTransform to shader <<--- HARDCODED
	GLuint transformID = glGetUniformLocation(object.m_programID, "modelTransform");
	glUniformMatrix4fv(transformID, 1, GL_FALSE, &(object.m_modelTransform->transformMatrix)[0][0]);

	//send Light to shader <<--- HARDCODED
	GLuint lightID = glGetUniformLocation(object.m_programID, "directionalLight");
	glUniform3f(lightID, this->m_directionalLight.x, this->m_directionalLight.y, this->m_directionalLight.z);

	// Send textureSamplers to shader
	for (int samplerIndex = 0; samplerIndex < object.m_textureSamplersVector.size(); samplerIndex++)
	{
		glActiveTexture(GL_TEXTURE0 + samplerIndex);
		glBindTexture(GL_TEXTURE_2D, object.m_textureSamplersVector.at(samplerIndex).second);

		glUniform1i(object.m_textureSamplersVector.at(samplerIndex).first, samplerIndex);
	}

	glBindVertexArray(object.m_vertexArrayID);

	// Draw VAO

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.m_elementBufferId); // Index buffer
	glDrawElements(object.m_renderType, object.m_toMeshTriangles->size(), GL_UNSIGNED_INT, (void*)0 ); // Draw Triangles
	//glDrawArrays(object.m_renderType, 0, (GLint)object.m_toMeshVertices->size());

	glBindVertexArray(0);

	glUseProgram(0);

	for (uint vboIndex = 0; vboIndex < object.m_vboIDVector.size(); vboIndex++)
	{
		glDisableVertexAttribArray(vboIndex);
	}
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

	if (!object.m_toMeshNormals->empty())
	{
		GenerateBufferObject<vec3>(object, &((*object.m_toMeshNormals)[0]), object.m_toMeshTangents->size() * sizeof(vec3), 3, layoutIndex);
		layoutIndex++;
	}

	if (!object.m_toMeshNormals->empty())
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

	if(isFullScreen)
	{
		monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		renderSize = vec2(videoMode->width,videoMode->height);
	}
	
	window = glfwCreateWindow(renderSize.x, renderSize.y,windowTitle, monitor,NULL);
	
	
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
	glfwSetCursorPos(window,renderSize.x/2, renderSize.y/2);
	
	//Neat grey background
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	
	return window;
}