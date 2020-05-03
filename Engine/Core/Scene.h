#pragma once

#include "StdInclude.h"
#include "BLASingleton.h"
#include "DataStructures/Tree.h"

// TODO: TODO: Remove this TODO
#include "Core/RenderingManager.h"
#include "Core/Timer.h"
#include "Core/GameObject.h"
#include "Core/ComponentContainers.h"
#include "Core/ComponentSystemsScheduler.h"

// TODO: mmm how about no ? + CameraComponent should be moved out of Core
#include "Core/CameraComponent.h"

namespace BLA
{
    class ComponentSystem;
    class Scene
    {
        BLA_DECLARE_EXPORTED_ACCESS_SINGLETON(Scene)

    public:

        friend class GameObject;

        Scene();

        BLACORE_API void Initialize(RenderingManager* renderingManager);
        BLACORE_API void Update();
        BLACORE_API void Clear();

        BLACORE_API GameObject CreateObject(GameObjectID name, const GameObject &parentObject = GameObject::InvalidReference());

        BLACORE_API GameObject FindObjectByName(blaString name);

        BLACORE_API GameComponent* AddComponent(GameObject object, GameComponentID componentId);

        template<class T>
        BLACORE_API T* AddComponent(GameObject object);

        // TODO: mmm how about no ?
        BLACORE_API CameraComponent* GetMainCamera();

        BLACORE_API void UpdateSceneTimer(blaF32 time);

        //GameObject PickGameObjectInScene(const Ray& inRay, ColliderComponent::CollisionContact& outContact);

        BLACORE_API void SetGameObjectParent(const GameObject& parent, const GameObject& child);
        BLACORE_API GameObject GetGameObjectParent(const GameObject& object);

        BLACORE_API blaVector<GameObject> GetObjects();

        BLACORE_API const blaVector<GameObjectID>& GetObjectsID() const;

        BLACORE_API GameComponent* GetComponentPerObject(GameComponentID componentId, GameObject obj);

        template<class T>
        BLACORE_API T* GetComponentPerObject(GameObject obj);

        BLACORE_API blaVector<GameComponent*> GetComponentsPerObject(GameObject obj);

        enum ESceneFlags
        {
            DIRTY_SCENE_STRUCTURE = 1 << 0,
        };

        BLACORE_API blaU32 GetSceneFlags() const;

        Timer m_sceneTimer;

        BLACORE_API RenderingManager* GetRenderingManager() const;

    private:

        typedef blaHashMap<GameObjectID, GameObject::GameObjectFlags, GameObjectID::Hasher> GameObjectFlagMap;
        typedef blaHashMap<GameObjectID, GameObjectID, GameObjectID::Hasher> GameObjectHierarchyMap;

        GameObjectFlagMap m_gameObjectsFlags;
        GameObjectHierarchyMap m_gameObjectHierarchy;

        ComponentSystemsScheduler m_componentSystemsScheduler;

        // TODO: mmm how about no ?
        CameraComponent* m_camera;

        RenderingManager* m_renderingManager;

        ComponentContainer m_componentContainer;

        blaVector<GameObjectID> m_validObjects;

        blaVector<GameComponent*> m_toInitialize;

        blaU32 m_sceneFlags;
    };
}
