#include "AssetsManager.h"

#define FOURCC_DXT1 0x31545844 // "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // "DXT5" in ASCII

using namespace BLAengine;


AssetManager::AssetManager(void)
{
}


AssetManager::~AssetManager(void)
{
}

Asset* BLAengine::AssetManager::GetAsset(std::string filepath, AssetType& type)
{
	if (m_resourceMap.count(filepath))
	{
		//type = InvalidAsset;
		return nullptr;
	}

	std::pair<AssetType, uint32_t> asset = m_resourceMap[filepath];

	type = asset.first;
	uint32_t assetIndx = asset.second;

	switch (type)
	{
		case AssetType::TriangleMeshAsset:
		{
			TriangleMesh* meshPtr = m_triangleMeshesInMemory.at(assetIndx);
			if (Asset* assetPtr = (Asset*) dynamic_cast<Asset*>(meshPtr))
			{
				return assetPtr;
			}
		}
		case AssetType::TextureAsset:
		{
			Texture2D* texturePtr = m_textures2DInMemory.at(assetIndx);
			if (Asset* assetPtr = (Asset*) dynamic_cast<Asset*>(texturePtr))
			{
				return assetPtr;
			}
		}
		case AssetType::MaterialAsset:
		{
			Material* matPtr = m_materialsInMemory.at(assetIndx);
			if (Asset* assetPtr = (Asset*) dynamic_cast<Asset*>(matPtr))
			{
				return assetPtr;
			}
		}
	}

}

bool BLAengine::AssetManager::SaveScene(Scene* scene)
{
	SceneSerializer sceneSerializer;

	sceneSerializer.FromScene(scene);

	cout << "Scene obj vector " << scene->GetObjects().size() << "\n";

	std::ofstream fs;
	fs.open(string("test_scene.xml"));

	if (!fs.is_open())
	{
		cout << "Could not Write on file " << "Test_scene " << "\n";
	}

	cereal::XMLOutputArchive output(fs);

	output(cereal::make_nvp("Scene", sceneSerializer));

	return true;
}

bool AssetManager::LoadTriangleMesh(std::string filepath)
{
	if (m_resourceMap.count(filepath))
	{
		cout << "There already exists a file named: " << filepath << "\n";
		return false;
	}

	TriangleMeshSerializer meshSerializer;


	std::fstream fs;
	fs.open(filepath, std::fstream::in | std::fstream::binary);

	if (!fs.is_open())
	{
		cout << "Could not load mesh: " << filepath << "\n";
		return false;
	}

	cereal::BinaryInputArchive input(fs);

	input(meshSerializer);

	TriangleMesh* triangleMesh = meshSerializer.BuildMesh();
	m_triangleMeshesInMemory.push_back(triangleMesh);

	uint32_t indx = m_triangleMeshesInMemory.size() - 1;

	m_resourceMap[filepath] = std::pair<AssetType, uint32_t>(AssetType::TriangleMeshAsset, indx);

	return true;
}

bool AssetManager::SaveTriangleMesh(TriangleMesh* mesh)
{
	TriangleMeshSerializer meshSerializer;

	meshSerializer.BuildFromMesh(mesh);

	std::fstream fs;
	fs.open(mesh->GetName(), std::fstream::out | std::fstream::binary);
	
	if (!fs.is_open())
	{
		cout << "Could not Write on file " << mesh->GetName() << "\n";
	}
	
	cereal::BinaryOutputArchive output(fs);
	
	output(meshSerializer);
	
	return true;
}

