#pragma once
#include "../../Common/StdInclude.h"
#include "../../Common/System.h"
#include "../../AssetsImport/MeshSerializer.h"
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

		Asset* GetAsset(std::string filepath, AssetType &type);

		bool LoadAsset(std::string filepath, AssetType type);
		bool SaveAsset(std::string filepath, AssetType type);

	private:

		bool LoadTriangleMesh(std::string filepath);
		bool LoadTexture(std::string filepath);
		bool loadMaterial(std::string filepath);

		bool SaveMaterial(std::string filepath, Material* mat);
		bool SaveTexture(std::string filepath, Texture2D* mesh);
		bool SaveTriangleMesh(std::string filepath, TriangleMesh* mesh);

		map<std::string, std::pair<AssetType, uint32_t>> m_resourceMap;

		vector<TriangleMesh*> m_triangleMeshesInMemory;
		vector<Material*> m_materialsInMemory;
		vector<Texture2D*> m_textures2DInMemory;
	};

}
