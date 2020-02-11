#pragma once

#include <StdInclude.h>
#include <BLASingleton.h>

//TODO: Create a ''system'' abstraction and the scene will enumarate and update all systems
//TODO: Systems should register themselves if they are compiled and only the non empty systems are updated
//TODO: This way I don't have to include non Core stuff in core ... (it's bad)
#include <Core/CameraComponent.h>
#include <Core/RenderingManager.h>
#include <Core/Timer.h>
#include <Maths/Ray.h>
#include "GameObject.h"

namespace BLAengine
{
    class BLACORE_API Scene
    {
        BLA_DECLARE_SINGLETON(Scene)

    public:
        friend class GameObject;

        Scene();

        void Initialize(RenderingManager* renderingManager);
        void Update();
        void Clear();

        GameObject CreateObject(GameObjectID name, const GameObject &parentObject = GameObject::InvalidReference());

        GameObject FindObjectByName(blaString name);

        GameComponent* AddComponent(GameObject object, GameComponentID componentId);

        template<class T>
        T* AddComponent(GameObject object);

        CameraComponent* GetMainCamera();

        void UpdateSceneTimer(blaF32 time);

        //GameObject PickGameObjectInScene(const Ray& inRay, ColliderComponent::CollisionContact& outContact);

        void SetGameObjectParent(const GameObject& parent, const GameObject& child);
        GameObject GetGameObjectParent(const GameObject& object);

        blaVector<GameObject> GetObjects();

        const blaVector<GameObjectID>& GetObjectsID() const;

        GameComponent* GetComponentPerObject(GameComponentID componentId, GameObject obj);

        template<class T>
        T* GetComponentPerObject(GameObject obj);

        blaVector<GameComponent*> GetComponentsPerObject(GameObject obj);

        enum ESceneFlags
        {
            DIRTY_SCENE_STRUCTURE = 1 << 0,
        };

        blaU32 GetSceneFlags() const { return m_sceneFlags; }

        Timer m_sceneTimer;

        RenderingManager* GetRenderingManager() { return m_renderingManager; }

    private:

        typedef blaHashMap<GameObjectID, GameObject::GameObjectFlags, GameObjectID::Hasher> GameObjectFlagMap;
        typedef blaHashMap<GameObjectID, GameObjectID, GameObjectID::Hasher> GameObjectHierarchyMap;

        GameObjectFlagMap m_gameObjectsFlags;
        GameObjectHierarchyMap m_gameObjectHierarchy;

        CameraComponent* m_camera;
        RenderingManager* m_renderingManager;

        // TODO: Temp structures, this is to improve into a proper system down the line
        blaHashMap<blaStringId, blaHashMap<GameObjectID, GameComponent*, GameObjectID::Hasher>, blaStringId::Hasher> m_components;
        blaVector<GameObjectID> m_validObjects;

        blaVector<GameComponent*> m_toInitialize;

        blaU32 m_sceneFlags;
    };

    template <class T>
    T* Scene::AddComponent(GameObject object)
    {
        T* gc = GameComponentRegistry::GetSingletonInstance()->__CreateComponent<T>(object);

        m_components[T::ms_componentDescriptor.GetName()][object] = gc;

        return gc;
    }

    template <class T>
    T* Scene::GetComponentPerObject(GameObject obj)
    {
        return static_cast<T*>(GetComponentPerObject(T::ms_componentDescriptor.GetName(), obj));
    }
}
