#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Transform* modelTransform):
	m_meshVertices(vector<vec3>()),
	m_meshNormals	(vector<vec3>()),
	m_meshUVs		(vector<vec2>()),
	m_vboIDVector	(vector<pair<GLuint,pair<GLuint,GLuint>>>()),
	m_textureSamplersVector (vector<pair<GLuint,GLuint>>()),
	m_renderType  (GL_TRIANGLES),
	m_directionalLight(vec3(0))
{
	this->m_modelTransform = modelTransform;

	//Generate a VAObject for the mesh Renderer.
	glGenVertexArrays(1, &m_vertexArrayID);

}


MeshRenderer::~MeshRenderer(void)
{
	CleanUp();
}

string MeshRenderer::ToString()
{
	return ("MRenderer: " + m_meshVertices.size()) ; 
}

bool MeshRenderer::AssignMaterial(const char* name)
{
	extern SharedRessources* sharedRessources;
	m_programID = sharedRessources->GetMaterial(name);
	if(m_programID != 0)
	{
		m_matrixID = glGetUniformLocation(m_programID, "MVP");
		return true;
	}
	return false;
}

bool MeshRenderer::LoadTextureSample(const char* textureRessourceName,const char* sampleName)
{
	extern SharedRessources* sharedRessources;
	if(m_programID != 0)
	{
		GLuint textureID = glGetUniformLocation(m_programID, sampleName);
		GLuint textureRessource = sharedRessources->GetTexture(textureRessourceName);

		m_textureSamplersVector.push_back(pair<GLuint,GLuint>(textureID,textureRessource));

		return true;
	}
	return false;
}

bool MeshRenderer::IsMeshValid()
{
	bool check = true;
	if(m_meshVertices.empty())
	{
		check = false;
	}
	
	return check;
}

bool MeshRenderer::Draw(const mat4 projection,const mat4 view)
{
	glUseProgram(m_programID);

	mat4 MVP = projection * view * (m_modelTransform->transformMatrix);
	glUniformMatrix4fv(m_matrixID, 1, GL_FALSE, &MVP[0][0]);

	//send modelTransform to shader <<--- HARDCODED
	GLuint transformID = glGetUniformLocation(m_programID, "modelTransform");
	glUniformMatrix4fv(transformID, 1, GL_FALSE, &(m_modelTransform->transformMatrix)[0][0]);

	//send Light to shader <<--- HARDCODED
	GLuint lightID = glGetUniformLocation(m_programID, "directionalLight");
	glUniform3f(lightID,m_directionalLight.x,m_directionalLight.y,m_directionalLight.z);

	// Send textureSamplers to shader
	for(int samplerIndex = 0; samplerIndex < m_textureSamplersVector.size(); samplerIndex++)
	{
		glActiveTexture(GL_TEXTURE0 + samplerIndex);
		glBindTexture(GL_TEXTURE_2D, m_textureSamplersVector.at(samplerIndex).second);

		glUniform1i(m_textureSamplersVector.at(samplerIndex).first, samplerIndex);
	}

	glBindVertexArray (m_vertexArrayID);

	// Draw VAO
	glDrawArrays(m_renderType,0, m_meshVertices.size());

	glBindVertexArray (0);

	glUseProgram(0);

	for(uint vboIndex = 0; vboIndex < m_vboIDVector.size() ; vboIndex++)
	{
		glDisableVertexAttribArray(vboIndex);
	}
	return true;
}

// OpenGL Buffers related Code:
bool MeshRenderer::GenerateArrays()
{
	//HARDCODED, suits:
	// Input vertex data, different for all executions of this shader.
	//layout(location = 0) in vec3 vertexPosition_modelspace;
	//layout(location = 1) in vec2 vertexUV;
	//layout(location = 2) in vec3 vertexNormals;
	bool success = false;

	int layoutIndex = 0;

	if(IsMeshValid())
	{
		GenerateBufferObject<vec3>(&(m_meshVertices[0]),m_meshVertices.size() * sizeof(vec3), 3,layoutIndex);
		layoutIndex++;
		if( !m_meshUVs.empty() )
		{
			GenerateBufferObject<vec2>(&(m_meshUVs[0]),m_meshUVs.size() * sizeof(vec2),2,layoutIndex);
			layoutIndex++;
		}

		if( !m_meshNormals.empty() )
		{
			GenerateBufferObject<vec3>(&(m_meshNormals[0]),m_meshNormals.size() * sizeof(vec3),3,layoutIndex);
			layoutIndex++;
		}

		success = true;
	}
	else
	{
		cout << "Can't generate arrays for \"" <<  this << "\", Mesh Invalid.\n";
		success = false;
	}
	return success;
}

void MeshRenderer::GenerateVertexArrayID()
{
	// Vertex array of size 1 hardcoded for each MeshRenderer !
	glGenVertexArrays(1, &m_vertexArrayID);
}

bool MeshRenderer::CleanUp()
{
	// Clean up all buffers associated to the mesh Vertex Array ID
	CleanUpVBOs();

	// Clean texture samplers 
	for(GLushort texIndex = 0; texIndex < m_textureSamplersVector.size() ; texIndex++)
	{
		glDeleteTextures(1, &(m_textureSamplersVector.at(texIndex).first));
	}
	m_textureSamplersVector.clear();

	//Cleanup Shader
	//glDeleteProgram(m_programID);
	DestroyVertexArrayID();
	return true;
}

void MeshRenderer::CleanUpVBOs()
{
	// Cleanup VBO
	for(uint vboIndex = 0; vboIndex < m_vboIDVector.size() ; vboIndex++)
	{
		glDeleteBuffers(1,&((m_vboIDVector.at(vboIndex)).first));
	}
	m_vboIDVector.clear();
}

void MeshRenderer::DestroyVertexArrayID()
{
	// Vertex array of size 1 hardcoded for each MeshRenderer !
	glDeleteVertexArrays(1, &m_vertexArrayID);
}