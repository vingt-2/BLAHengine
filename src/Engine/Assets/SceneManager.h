#pragma once
#include <Common/StdInclude.h>
#include <Engine/Core/Scene.h>

#include "AssetsManager.h"

#define SCENE_SUBPATH "./Scenes/"

namespace BLAengine
{
    class BLACORE_API SceneManager
    {
    public:
        SceneManager(AssetManager* assetManager);

        bool SaveScene(blaString filepath, Scene* scene);

        Scene* LoadScene(blaString filepath);

        blaString GetCurrentSceneFilePath() const
        {
            return m_currentSceneFilePath;
        }

    private:
        AssetManager* m_assetManager;

        blaString m_currentSceneFilePath;
    };
}