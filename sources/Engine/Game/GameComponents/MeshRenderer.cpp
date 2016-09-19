#include "MeshRenderer.h"
using namespace BLAengine;

MeshRenderer::MeshRenderer(Transform* modelTransform):
	m_renderType    (GL_TRIANGLES),
	m_renderTicket  (0)
{
	this->m_mesh = NULL;
	this->m_renderData = NULL;
	this->m_modelTransform = modelTransform;
} 

MeshRenderer::~MeshRenderer(void)
{
	//CleanUp();
	m_renderData->~RenderData();
}

string MeshRenderer::ToString()
{
	return "MeshRenderer ToString PLACEHOLDER";
}

bool MeshRenderer::AssignTriangleMesh(TriangleMesh* mesh)
{
	m_mesh = mesh;
	m_renderData = m_mesh->GenerateRenderData();

	return true;
}

bool MeshRenderer::AssignMaterial(const char* materialName)
{
	string material = string(materialName);
	m_materials.push_back(material);

	return true;
}	

bool MeshRenderer::AssignTexture(const char* textureName, const char* handleName)
{
	string texture = string(textureName);
	string handle = string(handleName);
	pair<string, string> entry = pair<string, string>(texture, handle);
	m_textures.push_back(entry);
	
	return true;
}
