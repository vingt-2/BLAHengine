// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Core/Scene.h"

#define SCENE_SUBPATH "./Scenes/"

namespace BLA
{
    class SceneManager
    {
    public:
        BLACORE_API SceneManager();

        BLACORE_API bool SaveScene(blaString filepath, Scene* scene);

        BLACORE_API void LoadScene(blaString filepath);

        BLACORE_API blaString GetCurrentSceneFilePath() const;

    private:
        blaString m_currentSceneFilePath;
    };
}