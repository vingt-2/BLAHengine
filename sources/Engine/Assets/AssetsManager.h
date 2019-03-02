#pragma once
#include <Common/StdInclude.h>
#include <Common/System.h>

#include "MeshAsset.h"
#include "Material.h"
#include "Texture.h"

#define TRIANGLE_MESH_SUBPATH "./Cooked/TriangleMeshs/"
#define TEXTURE_SUBPATH "./Cooked/Textures/"
#define MATERIAL_SUBPATH "./Cooked/Materials/"

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

        int LoadCookedAssets();

        AssetType GetAsset(std::string filepath, Asset* &assetPtr);

        bool LoadTriangleMesh(std::string filepath);
        bool LoadTexture(std::string filepath);
        bool LoadMaterial(std::string filepath);

        bool SaveMaterial(Material* mat);
        bool SaveTexture(Texture2D* mesh);
        bool SaveTriangleMesh(MeshAsset* mesh);

    private:

        map<std::string, std::pair<AssetType, blaU32>> m_resourceMap;

        vector<MeshAsset*> m_triangleMeshesInMemory;
        vector<Material*> m_materialsInMemory;
        vector<Texture2D*> m_textures2DInMemory;
    };

}
