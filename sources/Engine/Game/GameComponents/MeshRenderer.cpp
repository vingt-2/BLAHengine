#include "MeshRenderer.h"
#include "../GameObject.h"
using namespace BLAengine;

MeshRenderer::MeshRenderer():
	m_renderType    (GL_TRIANGLES),
	m_renderTicket  (0),
	m_modelTransformMatrix(new mat4(0))
{
	this->m_mesh = nullptr;
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

mat4* MeshRenderer::GetTransformMatrix() const
{
	mat4* a = m_modelTransformMatrix;
	return a;
}

void MeshRenderer::Update()
{
	if (!m_parentObject)
		* m_modelTransformMatrix = mat4(0);
	else
		* m_modelTransformMatrix = m_parentObject->GetTransform().m_transformMatrix;
}
