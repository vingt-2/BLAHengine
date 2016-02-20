#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Transform* modelTransform):
	m_meshVertices  (vector<vec3>()),
	m_meshNormals	(vector<vec3>()),
	m_meshUVs		(vector<vec2>()),
	m_renderType    (GL_TRIANGLES),
	m_renderTicket  (0)
{
	this->m_modelTransform = modelTransform;
} 

MeshRenderer::~MeshRenderer(void)
{
	//CleanUp();
}

string MeshRenderer::ToString()
{
	return ("MRenderer: " + m_meshVertices.size()) ;
}

bool MeshRenderer::AssignMaterial(const char* materialName)
{
	extern SharedResources* sharedResources;

	string material = string(materialName);
	if (!sharedResources->GetMaterial(material.data()))
	{
		return false;
	}
	m_materials.push_back(material);

	return true;
}	

bool MeshRenderer::AssignTexture(const char* textureName, const char* handleName)
{
	extern SharedResources* sharedResources;

	string texture = string(textureName);
	string handle = string(handleName);
	if (!sharedResources->GetTexture(textureName))
	{
		return false;
	}
	pair<string, string> entry = pair<string, string>(texture, handle);
	m_textures.push_back(entry);
	
	return true;
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