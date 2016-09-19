#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "./Transform.h"
#include "../../Assets/AssetsManager.h"
#include "../../Assets/PolygonalMesh.h"

#define BLA_LINE_RENDER 0x0003

namespace BLAengine
{
	class BLACORE_API MeshRenderer
	{
	public:

		int m_renderTicket;

		Transform* m_modelTransform;

		TriangleMesh* m_mesh;
		RenderData* m_renderData;

		vector <pair<string, string>> m_textures;
		vector <string> m_materials;

		GLuint m_renderType;

		MeshRenderer(Transform* modelTransform);
		~MeshRenderer(void);

		bool AssignTriangleMesh(TriangleMesh* mesh);
		bool AssignMaterial(const char* name);
		bool AssignTexture(const char* textureName, const char* handleName);

		string ToString(void);
	};
}