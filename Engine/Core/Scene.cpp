// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Scene.h"

#include "Core/TransformComponent.h"
#include "Core/ComponentSystems.h"

namespace BLA::Core
{
    BLA_IMPLEMENT_SINGLETON(Scene)

        Scene::Scene() : m_sceneTimer(Timer(1))
    {
        this->m_camera = nullptr;
    }

    GameObject Scene::CreateObject(GameObjectID objectId, const GameObject& parentObject)
    {
        m_sceneFlags |= ESceneFlags::DIRTY_SCENE_STRUCTURE;
        GameObject ref = GameObject(objectId);
        m_validObjects.push_back(ref);

        AddComponent(ref, BlaStringId("TransformComponent"));

        return ref;
    }

    GameObject Scene::FindObjectByName(blaString name)
    {
        for (auto o : m_validObjects)
        {
            if (blaString(o) == name)
            {
                return o;
            }
        }
        return GameObject();
    }

    GameComponent* Scene::AddComponent(GameObject object, GameComponentID componentId)
    {
        // TODO: Obviously make this a bit better
        auto oit = std::find(m_validObjects.begin(), m_validObjects.end(), object);
        if (oit == m_validObjects.end())
        {
            CreateObject(object);
        }

        GameComponent* gc = m_componentContainer.AddComponent(object, componentId);

        BLA_ASSERT(gc);

        if (gc)
            m_toInitialize.push_back(gc);

        return gc;
    }

    bool Scene::RemoveComponent(GameObject object, GameComponentID componentId)
    {
        if (GameComponent* component = GetComponentPerObject(componentId, object))
        {
            component->Shutdown();
            return m_componentContainer.RemoveComponent(object, componentId);
        }
        return false;
    }

    void Scene::UpdateSceneTimer(blaF32 time)
    {
        m_sceneTimer.Update(time);
    }

    void Scene::Initialize()
    {

        m_componentSystemsScheduler.RebuildScheduler(ComponentSystemsRegistry::GetSingletonInstanceRead());
    }

    void Scene::Update()
    {
        m_sceneFlags = 0;

        for (GameComponent* gc : m_toInitialize)
        {
            gc->Init();
        }
        m_toInitialize.clear();

        m_componentSystemsScheduler.RefreshSchedulerState();

        const ComponentSystemsRegistry* systemsRegistry = ComponentSystemsRegistry::GetSingletonInstanceRead();

        blaStringId systemToExecute;
        while (m_componentSystemsScheduler.GetNextJob(systemToExecute))
        {
            const ComponentSystem* system = systemsRegistry->GetSystemPointer(systemToExecute);

            // System validation
            blaVector<blaStringId> iSystem = system->GetInputComponents();
            blaVector<blaStringId> oSystem = system->GetOutputComponents();

            blaVector<GameObjectID> systemValidObjs;

            for (GameObjectID object : m_validObjects)
            {
                bool validCall = true;

                for (blaStringId c : iSystem)
                {
                    if (!m_componentContainer.GetComponentPerObject(c, object))
                    {
                        validCall = false;
                        break;
                    }
                }
                if (!validCall) continue;
                for (blaStringId c : oSystem)
                {
                    if (!m_componentContainer.GetComponentPerObject(c, object))
                    {
                        validCall = false;
                        break;
                    }
                }
                if (!validCall) continue;

                systemValidObjs.push_back(object);
            }

            system->ExecuteSystem(systemValidObjs, m_componentContainer.GetComponentSystemIOInterface());
            m_componentSystemsScheduler.NotifyCompletion(systemToExecute);
        }
    }

    void Scene::Clear()
    {
        m_componentContainer.Clear();

        m_validObjects.clear();

        m_camera = nullptr;

        m_sceneFlags |= DIRTY_SCENE_STRUCTURE;

        m_toInitialize.clear();
    }

    CameraComponent* Scene::GetMainCamera()
    {
        if (m_camera == nullptr)
        {
            for (GameObject o : m_validObjects)
            {
                if (CameraComponent* camera = GetComponentPerObject<CameraComponent>(o))
                {
                    if (!camera->m_isShadowMapCamera)
                    {
                        m_camera = camera;
                        break;
                    }
                }
            }
        }
        BLA_ASSERT(m_camera);
        return m_camera;
    }

    //GameObject Scene::PickGameObjectInScene(const Ray& inRay, ColliderComponent::CollisionContact& outContact)
    //{
    //    float minDistance = MAX_NORMAL_FLOAT;
    //    GameObject pickedObject;
    //    ColliderComponent::CollisionContact contactPoint;
    //
    //    for (blaU32 i = 0; i < m_sceneObjectsVector.size(); ++i)
    //    {
    //        GameObject& obj = m_sceneObjectsVector[i]     ;
    //
    //        ColliderComponent* collider = obj.GetComponent<ColliderComponent>();
    //
    //        if (collider == nullptr)
    //            continue;
    //
    //        const blaBool collision = collider->CollideWithRay(inRay, contactPoint);
    //
    //        if (!collision)
    //            continue;
    //
    //        const float distance = glm::dot(contactPoint.m_colPositionW - inRay.m_origin, inRay.m_direction);
    //
    //        if (distance > 0 && distance < minDistance)
    //        {
    //            minDistance = distance;
    //            pickedObject = GameObject(i, &m_sceneObjectsVector);
    //            outContact = contactPoint;
    //        }
    //    }
    //
    //    return GameObject();
    //}

    void Scene::SetGameObjectParent(const GameObject& parent, const GameObject& child)
    {
        if (parent.IsValid())
        {
            m_gameObjectHierarchy[child] = parent;
        }
        else
        {
            GameObjectHierarchyMap::iterator it = m_gameObjectHierarchy.find(child);
            if (it != m_gameObjectHierarchy.end())
                m_gameObjectHierarchy.erase(it);
        }
    }

    GameObject Scene::GetGameObjectParent(const GameObject& object)
    {
        GameObjectHierarchyMap::iterator it = m_gameObjectHierarchy.find(object);
        if (it != m_gameObjectHierarchy.end())
        {
            return it->second;
        }
        return GameObject();
    }

    blaVector<GameObject> Scene::GetObjects()
    {
        blaVector<GameObject> ret;
        for (int i = 0; i < m_validObjects.size(); ++i)
        {
            ret.push_back(GameObject(m_validObjects[i]));
        }

        return ret;
    }

    const blaVector<GameObjectID>& Scene::GetObjectsID() const
    {
        return m_validObjects;
    }

    GameComponent* Scene::GetComponentPerObject(GameComponentID componentId, GameObject obj)
    {
        return m_componentContainer.GetComponentPerObject(componentId, obj);
    }

    blaVector<GameComponent*> Scene::GetComponentsPerObject(GameObject obj)
    {
        return m_componentContainer.GetComponentsPerObject(obj);
    }

    blaU32 Scene::GetSceneFlags() const
    {
        return m_sceneFlags;
    }

    template <class T>
    T* Scene::AddComponent(GameObject object)
    {
        return static_cast<T*>(AddComponent(object, T::ms_componentDescriptor.m_typeID));
    }

    template <class T>
    T* Scene::GetComponentPerObject(GameObject object)
    {
        return static_cast<T*>(GetComponentPerObject(T::ms_componentDescriptor.m_typeID, object));
    }
}
