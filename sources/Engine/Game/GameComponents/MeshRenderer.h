#pragma once
#include "./Transform.h"
#include "../../AssetsManager/SharedResources.h"
#include "../../AssetsManager/PolygonalMesh.h"
#define BLA_LINE_RENDER 0x0003

class MeshRenderer
{
public:

	int m_renderTicket;

	Transform* m_modelTransform;

	PolygonalMesh* m_mesh;

	vector <pair<string,string>> m_textures;
	vector <string> m_materials;

	GLuint m_renderType;
	
	MeshRenderer(Transform* modelTransform, PolygonalMesh* mesh);
	~MeshRenderer(void);

	bool AssignMaterial(const char* name);
	bool AssignTexture(const char* textureName, const char* handleName);

	string ToString(void);
};

