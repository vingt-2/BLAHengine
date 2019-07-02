#pragma once
#include <Common/StdInclude.h>
#include <Common/System.h>
#include <Common/BLASingleton.h>

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
        BLA_DECLARE_SINGLETON(AssetManager);

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

        AssetType GetAsset(blaString assetName, Asset* &assetPtr);

        bool LoadTriangleMesh(blaString filepath);
        bool LoadTexture(blaString filepath);
        bool LoadMaterial(blaString filepath);

        bool SaveMaterial(Material* mat);
        bool SaveTexture(Texture2D* mesh);
        bool SaveTriangleMesh(MeshAsset* mesh);

    private:

        blaMap<blaString, blaPair<AssetType, blaU32>> m_resourceMap;

        blaVector<MeshAsset*> m_triangleMeshesInMemory;
        blaVector<Material*> m_materialsInMemory;
        blaVector<Texture2D*> m_textures2DInMemory;
    };

}
