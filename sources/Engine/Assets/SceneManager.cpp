#include "SceneManager.h"
#include <AssetsImport/InternalFormats/SceneSerializer.h>
#include <Engine/Game/GameComponents/ColliderComponent.h>
using namespace BLAengine;

BLAengine::SceneManager::SceneManager(AssetManager * assetManager)
{
    m_assetManager = assetManager;
    m_currentSceneFilePath = "";
}

bool SceneManager::SaveScene(string filepath, Scene* scene)
{
    SceneSerializer sceneSerializer;

    sceneSerializer.FromScene(scene);

    //cout << "Scene obj vector " << scene->GetObjects().size() << "\n";

    std::ofstream fs;
    fs.open(filepath);

    if (!fs.is_open())
    {
        cout << "Could not Write on file " << filepath << "\n";
    }

    cereal::JSONOutputArchive output(fs);

    output(cereal::make_nvp("Scene", sceneSerializer));

    m_currentSceneFilePath = filepath;

    return true;
}

Scene* SceneManager::LoadScene(std::string filepath)
{
    SceneSerializer sceneSerializer;

    std::ifstream fs;
    fs.open(filepath);

    cereal::JSONInputArchive input(fs);

    input(sceneSerializer);

    vector<GameObjectSerializer> objsSer = sceneSerializer.GetGameObjectVector();

    Scene* newScene = new Scene();

    for (auto gObjectSer : objsSer)
    {
        GameObjectReference gameObject = newScene->CreateObject(gObjectSer.GetName());

        gameObject->SetTransform(gObjectSer.GetTransform());

        vector<std::shared_ptr<GameComponentSerializer>> compSers = gObjectSer.GetComponentVector();
        for (auto compSer : compSers)
        {
            if (std::shared_ptr<MeshRendererSerializer> meshRenderSer = std::dynamic_pointer_cast<MeshRendererSerializer>(compSer))
            {
                MeshRendererComponent* meshRenderer = BLA_CREATE_COMPONENT(MeshRendererComponent, gameObject);

                Asset* triMeshAsset = nullptr;
                if (m_assetManager->GetAsset(meshRenderSer->GetMeshName(), triMeshAsset) == AssetManager::AssetType::TriangleMeshAsset)
                {
                    meshRenderer->AssignTriangleMesh((TriangleMesh*)triMeshAsset);
                }
                else
                {
                    cout << "Couldn't find TriangleMesh: " << meshRenderSer->GetMeshName() << " in AssetManager.\n";
                }

                int matIndex = 0;
                for (auto materialName : meshRenderSer->GetMaterialNames())
                {
                    Asset* materialAsset = nullptr;
                    if (m_assetManager->GetAsset(materialName, materialAsset) == AssetManager::AssetType::MaterialAsset)
                    {
                        meshRenderer->AssignMaterial((Material*)materialAsset, matIndex);
                    }
                    else
                    {
                        cout << "Couldn't find Material: " << materialName << "in AssetManager.\n";
                    }
                    matIndex++;
                }

                //
                // Automatically add a collider for now...
                // 

                MeshColliderComponent* collider = BLA_CREATE_COMPONENT(MeshColliderComponent, gameObject);

                collider->SetColliderMesh((TriangleMesh*)triMeshAsset);
            }
        }
    }

    m_currentSceneFilePath = filepath;

    return newScene;
}