#pragma once
#include "../../Common/StdInclude.h"
#include "../../Common/System.h"
#include "../../AssetsImport/MeshSerializer.h"
#include "../../AssetsImport/TextureSerializer.h"
#include "../../AssetsImport/MaterialSerializer.h"
#include "Material.h"
#include "Texture.h"

namespace BLAengine
{
	class BLACORE_API AssetManager
	{
	public:

		enum AssetType
		{
			TriangleMeshAsset,
			TextureAsset,
			MaterialAsset,
			InvalidAsset
		};

		AssetManager(void);
		~AssetManager(void);

		AssetType GetAsset(std::string filepath, Asset* &assetPtr);

		bool LoadTriangleMesh(std::string filepath);
		bool LoadTexture(std::string filepath);
		bool LoadMaterial(std::string filepath);

		bool SaveMaterial(Material* mat);
		bool SaveTexture(Texture2D* mesh);
		bool SaveTriangleMesh(TriangleMesh* mesh);

	private:

		map<std::string, std::pair<AssetType, uint32_t>> m_resourceMap;

		vector<TriangleMesh*> m_triangleMeshesInMemory;
		vector<Material*> m_materialsInMemory;
		vector<Texture2D*> m_textures2DInMemory;
	};

}
