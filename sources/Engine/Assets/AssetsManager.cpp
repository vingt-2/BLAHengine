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

AssetManager::AssetType BLAengine::AssetManager::GetAsset(std::string filepath, Asset* &assetPtr)
{
	AssetType type;
	if (!m_resourceMap.count(filepath))
	{
		type = InvalidAsset;
		return type;
	}

	std::pair<AssetType, uint32_t> asset = m_resourceMap[filepath];

	type = asset.first;
	uint32_t assetIndx = asset.second;

	switch (type)
	{
		case AssetType::TriangleMeshAsset:
		{
			TriangleMesh* meshPtr = m_triangleMeshesInMemory.at(assetIndx);
			if (assetPtr = (Asset*) dynamic_cast<Asset*>(meshPtr))
			{
				return TriangleMeshAsset;
			}
		}
		case AssetType::TextureAsset:
		{
			Texture2D* texturePtr = m_textures2DInMemory.at(assetIndx);
			if (assetPtr = (Asset*) dynamic_cast<Asset*>(texturePtr))
			{
				return TextureAsset;
			}
		}
		case AssetType::MaterialAsset:
		{
			Material* matPtr = m_materialsInMemory.at(assetIndx);
			if (assetPtr = (Asset*) dynamic_cast<Asset*>(matPtr))
			{
				return MaterialAsset;
			}
		}
	}

	return type;
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

bool AssetManager::LoadTexture(std::string filepath)
{
	if (m_resourceMap.count(filepath))
	{
		cout << "There already exists a file named: " << filepath << "\n";
		return false;
	}

	Texture2DSerializer texture2DSerializer;

	std::fstream fs;
	fs.open(filepath, std::fstream::in | std::fstream::binary);

	if (!fs.is_open())
	{
		cout << "Could not load Texture file: " << filepath << "\n";
		return false;
	}

	cereal::BinaryInputArchive input(fs);

	input(texture2DSerializer);

	Texture2D* texture2D = texture2DSerializer.BuildTexture();

	m_textures2DInMemory.push_back(texture2D);

	uint32_t indx = m_textures2DInMemory.size() - 1;

	m_resourceMap[filepath] = std::pair<AssetType, uint32_t>(AssetType::TextureAsset, indx);

	return true;
}

bool AssetManager::LoadMaterial(std::string filepath)
{
	if (m_resourceMap.count(filepath))
	{
		cout << "There already exists a file named: " << filepath << "\n";
		return false;
	}

	MaterialSerializer matSerializer;

	std::fstream fs;
	fs.open(filepath, std::fstream::in | std::fstream::binary);

	if (!fs.is_open())
	{
		cout << "Could not load Texture file: " << filepath << "\n";
		return false;
	}

	cereal::BinaryInputArchive input(fs);

	input(matSerializer);

	Material* material = matSerializer.BuildMaterial();

	m_materialsInMemory.push_back(material);

	uint32_t indx = m_materialsInMemory.size() - 1;

	m_resourceMap[filepath] = std::pair<AssetType, uint32_t>(AssetType::MaterialAsset, indx);

	return true;
}

bool BLAengine::AssetManager::SaveMaterial(Material * mat)
{
	MaterialSerializer matSerializer;

	matSerializer.FromMaterial(mat);

	std::fstream fs;
	fs.open(mat->GetName(), std::fstream::out | std::fstream::binary);

	if (!fs.is_open())
	{
		cout << "Could not Write on file " << mat->GetName() << "\n";
		return false;
	}

	cereal::BinaryOutputArchive output(fs);

	output(matSerializer);

	return true;
}

bool BLAengine::AssetManager::SaveTexture(Texture2D * tex)
{
	Texture2DSerializer texture2DSerializer;

	texture2DSerializer.FromTexture(tex);

	std::fstream fs;
	fs.open(tex->GetName(), std::fstream::out | std::fstream::binary);

	if (!fs.is_open())
	{
		cout << "Could not Write on file " << tex->GetName() << "\n";
		return false;
	}

	cereal::BinaryOutputArchive output(fs);

	output(texture2DSerializer);

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
		return false;
	}
	
	cereal::BinaryOutputArchive output(fs);
	
	output(meshSerializer);
	
	return true;
}

