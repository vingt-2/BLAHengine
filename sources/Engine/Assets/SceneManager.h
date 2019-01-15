#pragma once
#include <Common/StdInclude.h>
#include <Engine/Game/Scene.h>

#include "AssetsManager.h"

#define SCENE_SUBPATH "./Scenes/"

namespace BLAengine
{
    class BLACORE_API SceneManager
    {
    public:
        SceneManager(AssetManager* assetManager);

        bool SaveScene(std::string filepath, Scene* scene);

        Scene* LoadScene(std::string filepath);

    private:
        AssetManager* m_assetManager;
    };
}