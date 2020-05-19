#include "AssetsManager.h"

#include "System/Console.h"
#include "External/Files/dirent.h"

#include "Assets/MeshSerializer.h"
#include "Assets/TextureSerializer.h"
#include "Assets/MaterialSerializer.h"

#define FOURCC_DXT1 0x31545844 // "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // "DXT5" in ASCII

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(AssetManager);

AssetManager::AssetManager(void)
{
}


AssetManager::~AssetManager(void)
{
}

static blaVector<blaString> FilesInDir(blaString dirname)
{
    blaVector<blaString> result;
    DIR *dir;
    struct dirent *ent;

    dir = opendir(dirname.data());
    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            switch (ent->d_type)
            {
            case DT_REG:
                result.push_back(ent->d_name);
                break;
            }
        }
    }
    closedir(dir);
    return result;
}

blaIndex BLA::AssetManager::LoadCookedAssets()
{
    blaVector<blaString> triMeshNames = FilesInDir(TRIANGLE_MESH_SUBPATH);
    blaVector<blaString> matNames = FilesInDir(MATERIAL_SUBPATH);
    blaVector<blaString> textureNames = FilesInDir(TEXTURE_SUBPATH);

    for (auto n : triMeshNames)
    {
        this->LoadTriangleMesh(n);
    }
    for (auto n : matNames)
    {
        this->LoadMaterial(n);
    }
    for (auto n : textureNames)
    {
        this->LoadTexture(n);
    }

    return triMeshNames.size() + matNames.size() + textureNames.size();
}

AssetManager::AssetType AssetManager::GetAsset(blaString assetName, Asset* &assetPtr)
{
    AssetType type;
    if (!m_resourceMap.count(assetName))
    {
        type = InvalidAsset;
        return type;
    }

    blaPair<AssetType, blaIndex> asset = m_resourceMap[assetName];

    type = asset.first;
    const blaIndex assetIndx = asset.second;

    switch (type)
    {
    case AssetType::TriangleMeshAsset:
    {
        MeshAsset* meshPtr = m_triangleMeshesInMemory.at(assetIndx);
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

bool AssetManager::LoadTriangleMesh(blaString filepath)
{
    if (m_resourceMap.count(filepath))
    {
        cout << "There already exists a file named: " << filepath << "\n";
        return false;
    }

    TriangleMeshSerializer meshSerializer;


    std::fstream fs;
    fs.open(blaString(TRIANGLE_MESH_SUBPATH) + filepath, std::fstream::in | std::fstream::binary);

    if (!fs.is_open())
    {
        cout << "Could not load mesh: " << blaString(TRIANGLE_MESH_SUBPATH) + filepath << "\n";
        return false;
    }

    cereal::BinaryInputArchive input(fs);

    input(meshSerializer);

    MeshAsset* triangleMesh = meshSerializer.BuildMesh();
    m_triangleMeshesInMemory.push_back(triangleMesh);

    blaIndex indx = m_triangleMeshesInMemory.size() - 1;

    m_resourceMap[filepath] = blaPair<AssetType, blaIndex>(AssetType::TriangleMeshAsset, indx);

    return true;
}

bool AssetManager::LoadTexture(blaString filepath)
{
    if (m_resourceMap.count(filepath))
    {
        cout << "There already exists a file named: " << filepath << "\n";
        return false;
    }

    Texture2DSerializer texture2DSerializer;

    std::fstream fs;
    fs.open(blaString(TEXTURE_SUBPATH) + filepath, std::fstream::in | std::fstream::binary);

    if (!fs.is_open())
    {
        cout << "Could not load Texture file: " << blaString(TEXTURE_SUBPATH) + filepath << "\n";
        return false;
    }

    cereal::BinaryInputArchive input(fs);

    input(texture2DSerializer);

    Texture2D* texture2D = texture2DSerializer.BuildTexture();

    m_textures2DInMemory.push_back(texture2D);

    blaIndex indx = m_textures2DInMemory.size() - 1;

    m_resourceMap[filepath] = blaPair<AssetType, blaIndex>(AssetType::TextureAsset, indx);

    return true;
}

bool AssetManager::LoadMaterial(blaString filepath)
{
    if (m_resourceMap.count(filepath))
    {
        cout << "There already exists a file named: " << filepath << "\n";
        return false;
    }

    MaterialSerializer matSerializer;

    std::fstream fs;
    fs.open(blaString(MATERIAL_SUBPATH) + filepath, std::fstream::in | std::fstream::binary);

    if (!fs.is_open())
    {
        cout << "Could not load Texture file: " << blaString(MATERIAL_SUBPATH) + filepath << "\n";
        return false;
    }

    cereal::BinaryInputArchive input(fs);

    input(matSerializer);

    Material* material = matSerializer.BuildMaterial();

    m_materialsInMemory.push_back(material);

    blaIndex indx = m_materialsInMemory.size() - 1;

    m_resourceMap[filepath] = blaPair<AssetType, blaIndex>(AssetType::MaterialAsset, indx);

    return true;
}

bool AssetManager::SaveMaterial(Material * mat)
{
    MaterialSerializer matSerializer;

    matSerializer.FromMaterial(mat);

    std::fstream fs;
    fs.open(blaString(MATERIAL_SUBPATH) + mat->GetName(), std::fstream::out | std::fstream::binary);

    if (!fs.is_open())
    {
        cout << "Could not Write on file " << blaString(MATERIAL_SUBPATH) + mat->GetName() << "\n";
        return false;
    }

    cereal::BinaryOutputArchive output(fs);

    output(matSerializer);

    return true;
}

bool BLA::AssetManager::SaveTexture(Texture2D * tex)
{
    Texture2DSerializer texture2DSerializer;

    texture2DSerializer.FromTexture(tex);

    std::fstream fs;
    fs.open(blaString(TEXTURE_SUBPATH) + tex->GetName(), std::fstream::out | std::fstream::binary);

    if (!fs.is_open())
    {
        cout << "Could not Write on file " << blaString(TEXTURE_SUBPATH) + tex->GetName() << "\n";
        return false;
    }

    cereal::BinaryOutputArchive output(fs);

    output(texture2DSerializer);

    return true;
}

bool AssetManager::SaveTriangleMesh(MeshAsset* mesh)
{
    TriangleMeshSerializer meshSerializer;

    meshSerializer.BuildFromMesh(mesh);

    std::fstream fs;
    fs.open(blaString(TRIANGLE_MESH_SUBPATH) + mesh->GetName(), std::fstream::out | std::fstream::binary);

    if (!fs.is_open())
    {
        cout << "Could not Write on file " << blaString(TRIANGLE_MESH_SUBPATH) + mesh->GetName() << "\n";
        return false;
    }

    cereal::BinaryOutputArchive output(fs);

    output(meshSerializer);

    return true;
}

DefineConsoleCommand(void, LoadTextureBMP, blaString name, blaString path)
{
    Texture2D* texture;
    texture = TextureImport::LoadBMP(name, path);
    AssetManager::GetSingletonInstance()->SaveTexture(texture);
    delete texture;
}

DefineConsoleCommand(void, LoadTextureTGA, blaString name, blaString path)
{
    Texture2D* texture;
    texture = TextureImport::LoadTGA(name, path);
    AssetManager::GetSingletonInstance()->SaveTexture(texture);
    delete texture;
}