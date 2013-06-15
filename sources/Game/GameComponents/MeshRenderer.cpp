#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Transform* modelTransform):
	m_meshVertices(vector<vec3>()),
	m_meshNormals	(vector<vec3>()),
	m_meshUVs		(vector<vec2>()),
	m_vboIDVector	(vector<pair<GLuint,pair<GLuint,GLuint>>>()),
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

bool MeshRenderer::Draw(const mat4 projection,const mat4 view)
{
	glUseProgram(m_programID);

	mat4 MVP = projection * view * (m_modelTransform->transformMatrix);
	glUniformMatrix4fv(m_matrixID, 1, GL_FALSE, &MVP[0][0]);


	GLuint transformID = glGetUniformLocation(m_programID, "modelTransform");
	glUniformMatrix4fv(transformID, 1, GL_FALSE, &(m_modelTransform->transformMatrix)[0][0]);

	if(m_diffuseTexture != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(m_textureID, 0);
	}

	GLuint lightID = glGetUniformLocation(m_programID, "directionalLight");
	glUniform3f(lightID,m_directionalLight.x,m_directionalLight.y,m_directionalLight.z);

	for(uint vboIndex = 0; vboIndex < m_vboIDVector.size() ; vboIndex++)
	{
		glEnableVertexAttribArray(vboIndex);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboIDVector.at(vboIndex).first);
		glVertexAttribPointer
		(
			m_vboIDVector.at(vboIndex).second.first,	// attribute
			m_vboIDVector.at(vboIndex).second.second,	// size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			0,							// stride
			(void*)0					// array buffer offset
		);
	}

	// Draw VAO
	glDrawArrays(m_renderType,0, m_meshVertices.size());

	for(uint vboIndex = 0; vboIndex < m_vboIDVector.size() ; vboIndex++)
	{
		glDisableVertexAttribArray(vboIndex);
	}
	return true;
}

bool MeshRenderer::GenerateArrays()
{
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

bool MeshRenderer::CleanUp()
{
	// Cleanup VBO
	for(uint vboIndex = 0; vboIndex < m_vboIDVector.size() ; vboIndex++)
	{
		glDeleteBuffers(1,&((m_vboIDVector.at(vboIndex)).first));
	}
	m_vboIDVector.clear();

	//Cleanup Shader
	//glDeleteProgram(programID);
	glDeleteTextures(1, &m_textureID);
	glDeleteVertexArrays(1, &m_vertexArrayID);

	return true;
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

bool MeshRenderer::AssignTexture(const char* name)
{
	extern SharedRessources* sharedRessources;
	if(m_programID != 0)
	{
		m_textureID = glGetUniformLocation(m_programID, "texture");
		m_diffuseTexture = sharedRessources->GetTexture(name);
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