#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "./Transform.h"
#include "../../Assets/AssetsManager.h"
#include "../../Assets/Material.h"

#define BLA_LINE_RENDER 0x0003

namespace BLAengine
{
	class BLACORE_API MeshRenderer
	{
	public:

		int m_renderTicket;

		Transform* m_modelTransform;

		TriangleMesh* m_mesh;

		vector<Material*> m_materials;

		GLuint m_renderType;

		MeshRenderer(Transform* modelTransform);
		~MeshRenderer(void);

		bool AssignTriangleMesh(TriangleMesh* mesh);

		bool AssignMaterial(Material* material, int matIndx);

		string ToString(void);
	};
}