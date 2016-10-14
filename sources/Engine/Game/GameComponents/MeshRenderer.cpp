#include "MeshRenderer.h"
using namespace BLAengine;

MeshRenderer::MeshRenderer(Transform* modelTransform):
	m_renderType    (GL_TRIANGLES),
	m_renderTicket  (0)
{
	this->m_mesh = nullptr;
	this->m_modelTransform = modelTransform;
} 

MeshRenderer::~MeshRenderer(void)
{
	//CleanUp();
}

string MeshRenderer::ToString()
{
	return "MeshRenderer ToString PLACEHOLDER";
}

bool MeshRenderer::AssignTriangleMesh(TriangleMesh* mesh)
{
	m_mesh = mesh;

	return true;
}

bool MeshRenderer::AssignMaterial(Material* material, int matIndx)
{
	m_materials.push_back(material);

	return true;
}