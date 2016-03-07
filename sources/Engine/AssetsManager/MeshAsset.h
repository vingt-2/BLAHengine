#pragma once
#include "SharedResources.h"
#include "../../Std/Maths.h"


class MeshAsset
{
public:

	vector<vec3> m_meshVertices;
	vector<vec3> m_meshNormals;
	vector<vec3> m_meshTangents;
	vector<vec3> m_meshBiTangents;
	vector<vec2> m_meshUVs;

	MeshAsset();
	~MeshAsset();

	void ComputeTangents();
};