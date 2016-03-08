#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Transform* modelTransform):
	m_meshVertices  (vector<vec3>()),
	m_meshNormals	(vector<vec3>()),
	m_meshTangents  (vector<vec3>()),
	m_meshBiTangents(vector<vec3>()),
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

bool MeshRenderer::ComputeTangents()
{


	return true;
}

void MeshRenderer::NormalizeModelCoordinates()
{
	float minX = this->m_meshVertices[0].x;
	float maxX = this->m_meshVertices[0].x;

	float minY = this->m_meshVertices[0].y;
	float maxY = this->m_meshVertices[0].y;

	float minZ = this->m_meshVertices[0].z;
	float maxZ = this->m_meshVertices[0].z;

	vec3 centroid = vec3(0, 0, 0);

	for (auto v : this->m_meshVertices) 
	{
		minX = v.x < minX ? v.x : minX;
		maxX = v.x > maxX ? v.x : maxX;

		minY = v.y < minY ? v.y : minY;
		maxY = v.y > maxY ? v.y : maxY;

		minZ = v.z < minZ ? v.z : minZ;
		maxZ = v.z > maxZ ? v.z : maxZ;
		centroid += v;
	}

	centroid /= m_meshVertices.size();

	float correctX = maxX - minX;
	float correctY = maxY - minY;
	float correctZ = maxZ - minZ;

	for (int i = 0; i < m_meshVertices.size(); i++)
	{
		m_meshVertices[i] -= centroid;

		//m_meshVertices[i].x /= correctX;

		//m_meshVertices[i].y /= correctY;

		//m_meshVertices[i].z /= correctZ;
	}
}