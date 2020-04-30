#pragma once
#include <StdInclude.h>
#include <Core/Scene.h>

#define SCENE_SUBPATH "./Scenes/"

namespace BLA
{
    class BLACORE_API SceneManager
    {
    public:
        SceneManager();

        bool SaveScene(blaString filepath, Scene* scene);

        void LoadScene(blaString filepath);

        blaString GetCurrentSceneFilePath() const
        {
            return m_currentSceneFilePath;
        }

    private:
        blaString m_currentSceneFilePath;
    };
}