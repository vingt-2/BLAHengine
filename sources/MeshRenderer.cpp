#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(void):
		meshVertices(vector<vec3>()),
		meshNormals	(vector<vec3>()),
		meshUVs		(vector<vec2>())
{}


MeshRenderer::~MeshRenderer(void)
{}

string MeshRenderer::ToString()
{
	return ("MRenderer: " + meshVertices.size()) ; 
}