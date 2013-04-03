#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Transform* modelTransform):
	meshVertices(vector<vec3>()),
	meshNormals	(vector<vec3>()),
	meshUVs		(vector<vec2>()),
	vboIDVector	(vector<pair<GLuint,GLuint>>()),
	renderType  (GL_TRIANGLES)
{
	this->modelTransform = modelTransform;

	//Generate a VAO for the mesh Renderer.
	glGenVertexArrays(1, &vertexArrayID);

}


MeshRenderer::~MeshRenderer(void)
{
	CleanUp();
}

string MeshRenderer::ToString()
{
	return ("MRenderer: " + meshVertices.size()) ; 
}

bool MeshRenderer::Draw(const mat4 projection,const mat4 view)
{
	glUseProgram(programID);

	mat4 MVP = projection * view * (modelTransform->transformMatrix);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

	for(uint vboIndex = 0; vboIndex < vboIDVector.size() ; vboIndex++)
	{
		glEnableVertexAttribArray(vboIndex);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDVector.at(vboIndex).first);
		glVertexAttribPointer
		(
			vboIDVector.at(vboIndex).second,// attribute
			3,							// size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			0,							// stride
			(void*)0					// array buffer offset
		);
	}

	// Draw VAO
	glDrawArrays(renderType,0, meshVertices.size());

	for(uint vboIndex = 0; vboIndex < vboIDVector.size() ; vboIndex++)
	{
		glDisableVertexAttribArray(vboIndex);
	}
	return true;
}

void MeshRenderer::GenerateVertexArray(GLuint attributeNumber)
{
	glBindVertexArray(vertexArrayID);

	//TO BE RELOCATED WHEN WE SUPPORT TEXTURES

	// Load the texture
	//GLuint Texture = loadDDS("uvmap.DDS");
	//GLuint Texture = 0;

	// Get a handle for our "myTextureSampler" uniform
	//GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Load it into a VBO
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(vec3), &(meshVertices[0]), GL_STATIC_DRAW);

	pair<GLuint,GLuint> vertexVBO = pair<GLuint,GLuint>(vertexBuffer,attributeNumber);

	vboIDVector.push_back(vertexVBO);
}

void MeshRenderer::GenerateUVArray(GLuint attributeNumber)
{
	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, meshUVs.size() * sizeof(vec2), &(meshUVs[0]), GL_STATIC_DRAW);

	pair<GLuint,GLuint> uvsVBO = pair<GLuint,GLuint>(uvBuffer,attributeNumber);

	vboIDVector.push_back(uvsVBO);
}

bool MeshRenderer::GenerateArrays()
{
	GenerateVertexArray(0);
	if( !meshUVs.empty() )
	{
		GenerateUVArray(1);
	}
	return true;
}

bool MeshRenderer::CleanUp()
{
	// Cleanup VBO
	for(uint vboIndex = 0; vboIndex < vboIDVector.size() ; vboIndex++)
	{
		glDeleteBuffers(1,&((vboIDVector.at(vboIndex)).first));
	}
	vboIDVector.clear();

	//Cleanup Shader
	glDeleteProgram(programID);
	glDeleteTextures(1, &textureID);
	glDeleteVertexArrays(1, &vertexArrayID);

	return true;
}

bool MeshRenderer::AssignMaterial(const char* name)
{
	extern SharedRessources* sharedRessources;
	programID = sharedRessources->GetMaterial(name);
	if(programID != 0)
	{
		matrixID = glGetUniformLocation(programID, "MVP");
		return true;
	}
		return false;
}

bool MeshRenderer::AssignTexture(const char* name)
{
	extern SharedRessources* sharedRessources;
	programID = sharedRessources->GetMaterial(name);
	if(programID != 0)
	{
		return true;
	}
		return false;
}