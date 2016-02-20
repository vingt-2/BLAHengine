#pragma once
//#include "../../Std/std.h"
#include "./Transform.h"
#include "../../AssetsManager/SharedResources.h"
#define BLA_LINE_RENDER 0x0003

class MeshRenderer
{
public:
	Transform* m_modelTransform;

	vector<vec3> m_meshVertices;
	vector<vec3> m_meshNormals;
	vector<vec2> m_meshUVs;

	vector <pair<string,string>> m_textures;
	vector <string> m_materials;

	GLuint m_renderType;
	
	MeshRenderer(Transform* modelTransform);
	~MeshRenderer(void);

	bool AssignMaterial(const char* name);
	bool AssignTexture(const char* textureName, const char* handleName);

	string ToString(void);
	bool IsMeshValid();
};

