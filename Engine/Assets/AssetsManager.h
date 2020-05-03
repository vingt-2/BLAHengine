#pragma once
#include "StdInclude.h"
#include "System.h"
#include "BLASingleton.h"

#include "MeshAsset.h"
#include "Material.h"
#include "Texture.h"


#define TRIANGLE_MESH_SUBPATH "./Cooked/TriangleMeshs/"
#define TEXTURE_SUBPATH "./Cooked/Textures/"
#define MATERIAL_SUBPATH "./Cooked/Materials/"

namespace BLA
{
    class AssetManager
    {
        BLA_DECLARE_EXPORTED_ACCESS_SINGLETON(AssetManager);

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

        BLACORE_API blaIndex LoadCookedAssets();

        BLACORE_API AssetType GetAsset(blaString assetName, Asset* &assetPtr);

        BLACORE_API bool LoadTriangleMesh(blaString filepath);
        BLACORE_API bool LoadTexture(blaString filepath);
        BLACORE_API bool LoadMaterial(blaString filepath);

        BLACORE_API bool SaveMaterial(Material* mat);
        BLACORE_API bool SaveTexture(Texture2D* mesh);
        BLACORE_API bool SaveTriangleMesh(MeshAsset* mesh);

    private:

        blaMap<blaString, blaPair<AssetType, blaIndex>> m_resourceMap;

        blaVector<MeshAsset*> m_triangleMeshesInMemory;
        blaVector<Material*> m_materialsInMemory;
        blaVector<Texture2D*> m_textures2DInMemory;
    };

}